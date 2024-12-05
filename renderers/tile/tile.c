#include "tile.h"
#include "config.h"
#include "error.h"
#include "frame.h"
#include "init.h"
#include "mat4.h"
#include "memory.h"
#include "texture.h"
#include "util.h"
#include "vec2.h"
#include <string.h>
#include <vulkan/vulkan_core.h>


typedef struct {
    sm_vec2f pos;
    sm_vec2f uv;
} Vertex;

typedef struct {
    sm_mat4f proj;
    sm_mat4f view;
    sm_vec2f tilesize;
    sm_vec2f imgsize;
} Uniform;

static const Vertex verts[] = {
    {{-1.0f, -1.0f}, {0.0f, 0.0f}},
    {{ 1.0f, -1.0f}, {1.0f, 0.0f}},
    {{ 1.0f,  1.0f}, {1.0f, 1.0f}},
    {{-1.0f,  1.0f}, {0.0f, 1.0f}},
};

static const uint16_t indicies[] = {
    0, 1, 2, 2, 3, 0
};

ErrorCode TileInit(TileRenderer* r, RenderPass* p, u32 subpass, u32 width, u32 height) {

    PASS_CALL(CreateShaderProg("shaders/tile/tile.vert.spv", "shaders/tile/tile.frag.spv", &r->shader));
    
    DescriptorDetail descriptorConfigs[] = {
        {SR_DESC_UNIFORM, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,   0}, 
        {SR_DESC_STORAGE, VK_SHADER_STAGE_FRAGMENT_BIT, 0},
        {SR_DESC_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1}
    };
    PASS_CALL(CreateDescriptorSetConfig(&r->config, descriptorConfigs, ARRAY_SIZE(descriptorConfigs)));

    AttrConfig vconfig[] = {
        {.rate = VK_VERTEX_INPUT_RATE_VERTEX, .format = VK_FORMAT_R32G32_SFLOAT, .size = sizeof(sm_vec2f)},
        {.rate = VK_VERTEX_INPUT_RATE_VERTEX, .format = VK_FORMAT_R32G32_SFLOAT, .size = sizeof(sm_vec2f)},
    };
    VkVertexInputBindingDescription binds[1];
    VkVertexInputAttributeDescription attrs[2];
    PASS_CALL(CreateVertAttr(attrs, binds, vconfig, 2));

    VulkanVertexInput vin = {
        .attrs = attrs,
        .binding = binds[0],
        .size = 2
    };
    PASS_CALL(CreatePipelineConfig(&r->shader, VulkanVertToConfig(vin), &r->config, TRUE));

    PASS_CALL(CreatePipeline(&r->shader, &r->config, &r->pipeline, p, subpass)); 

    PASS_CALL(CreateStaticBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, verts, sizeof(verts), &r->verts));
    PASS_CALL(CreateStaticBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indicies, sizeof(indicies), &r->indicies));
    
    TileLoadSetFile(&r->tileset, "resources/textures/duck.jpg", width, height);
    

    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        PASS_CALL(CreateDynamicBuffer(width * height * sizeof(int), &r->tiledata[i], VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
        PASS_CALL(CreateDynamicBuffer(sizeof(Uniform), &r->uniform[i], VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));
    }

    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        PASS_CALL(SetBuffer(&r->config, SR_DESC_UNIFORM, (Buffer*)&r->uniform[i], 0, i));
        PASS_CALL(SetBuffer(&r->config, SR_DESC_STORAGE, (Buffer*)&r->tiledata[i], 1, i));
        PASS_CALL(SetImage(r->tileset.atlas.image.view, r->tileset.atlas.sampler, &r->config, 2, 0, i))
    }
    return SR_NO_ERROR;
}

void TileSetData(TileRenderer* r, int* data) {
    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        memcpy(r->tiledata[i].buffer, data, r->tileset.tilewide * r->tileset.tileheight * sizeof(int));
    }
}


void TileDrawFrame(TileRenderer* r, PresentInfo* p, u32 frame) {
    Uniform ub = (Uniform) {
        .proj = SM_MAT4_IDENTITY,
        .view = SM_MAT4_IDENTITY,
        .tilesize = (sm_vec2f){r->tileset.tilewide, r->tileset.tileheight},
        .imgsize = (sm_vec2f){r->tileset.width, r->tileset.height}
    };

    memcpy(r->uniform[frame].buffer, &ub, sizeof(Uniform));
    
    VkCommandBuffer cmdBuf = sr_context.cmd.buffer[frame];

    vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, r->pipeline.pipeline);

    VkBuffer bufs[] = {r->verts.vhandle};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmdBuf, 0, 1, bufs, offsets);
    vkCmdBindIndexBuffer(cmdBuf, r->indicies.vhandle, 0, VK_INDEX_TYPE_UINT16); 
    r->pipeline.view.x = 0.0f;
    r->pipeline.view.y = 0.0f;
    r->pipeline.view.width = p->swapchain.extent.width;
    r->pipeline.view.height = p->swapchain.extent.height;
    r->pipeline.view.minDepth = 0.0f;
    r->pipeline.view.maxDepth = 1.0f;
    vkCmdSetViewport(cmdBuf, 0, 1, &r->pipeline.view);

    r->pipeline.scissor.offset = (VkOffset2D){0, 0};
    r->pipeline.scissor.extent = p->swapchain.extent;
    vkCmdSetScissor(cmdBuf, 0, 1, &r->pipeline.scissor);

    vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, r->config.layout, 0, 1, &r->config.descrip.descriptorSet[frame], 0, NULL);

    vkCmdDrawIndexed(cmdBuf, 6, 1, 0, 0, 0);
}



ErrorCode TileGetSubpass(SubPass* s, Attachment* a, u32 start) {
    *s = (SubPass) {
        .numAttachments = 0,
        .colorAttachment = 0,
        .depthAttachment = -1,
        .firstAttachment = 0,
    };
    return SR_NO_ERROR;
}

void DestroyTile(TileRenderer* r) {
    vkDeviceWaitIdle(sr_device.l);
    DestroyStaticBuffer(&r->verts);
    DestroyStaticBuffer(&r->indicies);
    DestroyPipeline(&r->pipeline);
    DestroyShaderProg(&r->shader);
    DestroyPipelineConfig(&r->config);
    DestroyTexture(&r->tileset.atlas);
    VkDevice d = sr_device.l;
    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(d, r->uniform[i].vhandle, NULL);
        vkFreeMemory(d, r->uniform[i].mem, NULL);
        vkDestroyBuffer(d, r->tiledata[i].vhandle, NULL);
        vkFreeMemory(d, r->tiledata[i].mem, NULL);
    }
}
