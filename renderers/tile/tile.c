#include "tile.h"
#include "config.h"
#include "error.h"
#include "init.h"
#include "memory.h"
#include "util.h"
#include "vec2.h"
#include <vulkan/vulkan_core.h>


typedef struct {
    sm_vec2f pos;
    sm_vec2f uv;
} Vertex;

static const Vertex verts[] = {
    {{-0.5f, -0.5f}, {0.0f, 0.0f}},
    {{ 0.5f, -0.5f}, {1.0f, 0.0f}},
    {{ 0.5f,  0.5f}, {1.0f, 1.0f}},
    {{-0.5f,  0.5f}, {0.0f, 1.0f}},
};

static const uint16_t indicies[] = {
    0, 1, 2, 2, 3, 0
};

ErrorCode TileInit(TileRenderer* r, RenderPass* p, u32 subpass, u32 width, u32 height) {

    PASS_CALL(CreateShaderProg("shaders/tile/tile.vert.spv", "shaders/tile/tile.frag.spv", &r->shader));
    
    DescriptorDetail descriptorConfigs[] = {
        {SR_DESC_UNIFORM, VK_SHADER_STAGE_VERTEX_BIT,   0}, 
        {SR_DESC_STORAGE, VK_SHADER_STAGE_VERTEX_BIT, 0},
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

    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        PASS_CALL(CreateDynamicBuffer(width * height * sizeof(int), &r->tiledata[i], VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
        PASS_CALL(CreateDynamicBuffer(sizeof(sm_vec2f), &r->uniform[i], VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));
    }

    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        PASS_CALL(SetBuffer(&r->config, SR_DESC_UNIFORM, (Buffer*)&r->uniform[i], 0, i));
        PASS_CALL(SetBuffer(&r->config, SR_DESC_STORAGE, (Buffer*)&r->tiledata[i], 1, i));
    }
    return SR_NO_ERROR;
}


ErrorCode TileGetSubpass(SubPass* s, Attachment* a, u32 start) {
    *s = (SubPass) {
        .colorAttachment = 0,
        .depthAttachment = -1,
        .firstAttachment = 0,
        .numAttachments = 1
    };
    return SR_NO_ERROR;
}

void TileDestroy(TileRenderer* r) {
    vkDeviceWaitIdle(sr_device.l);
    DestroyStaticBuffer(&r->verts);
    DestroyStaticBuffer(&r->indicies);
    DestroyPipeline(&r->pipeline);
    DestroyShaderProg(&r->shader);
    DestroyPipelineConfig(&r->config);
    VkDevice d = sr_device.l;
    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(d, r->uniform[i].vhandle, NULL);
        vkFreeMemory(d, r->uniform[i].mem, NULL);
        vkDestroyBuffer(d, r->tiledata[i].vhandle, NULL);
        vkFreeMemory(d, r->tiledata[i].mem, NULL);
    }
}
