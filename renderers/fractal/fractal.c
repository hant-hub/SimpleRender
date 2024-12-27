#include "fractal.h"
#include "config.h"
#include "error.h"
#include "frame.h"
#include "init.h"
#include "memory.h"
#include "pipeline.h"
#include "util.h"
#include "vec2.h"
#include "vec3.h"
#include <string.h>
#include <vulkan/vulkan_core.h>


typedef struct {
    sm_vec2f pos;
    sm_vec2f size;
    sm_vec3f color;
    float zoom;
    sm_vec2f initial;
} Uniform;

typedef struct {
    sm_vec2f pos;
    sm_vec2f uv;
} Vertex;


static const Vertex verts[] = {
    {{-1.0f, -1.0f}, {-1.0f, -1.0f}},
    {{ 1.0f, -1.0f}, {1.0f, -1.0f}},
    {{ 1.0f,  1.0f}, {1.0f, 1.0f}},
    {{-1.0f,  1.0f}, {-1.0f, 1.0f}},
};

static const uint16_t indicies[] = {
    0, 1, 2, 2, 3, 0
};


ErrorCode FractalInit(FractalRenderer* r, char* prog, RenderPass* p, u32 subpass) {

    char vert[64];
    char frag[64];

    snprintf(vert, sizeof(vert), "shaders/fractal/%s/fractal.vert.spv", prog);
    snprintf(frag, sizeof(frag), "shaders/fractal/%s/fractal.frag.spv", prog);

    PASS_CALL(CreateShaderProg(vert, frag, &r->shader));

    DescriptorDetail layout[] = {
        {SR_DESC_UNIFORM, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0}, 
    };
    PASS_CALL(CreateDescriptorSetConfig(&r->config, layout, ARRAY_SIZE(layout)));

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
    PASS_CALL(CreatePipelineConfig(&r->shader, VulkanVertToConfig(vin), &r->config, FALSE));
    PASS_CALL(CreatePipeline(&r->shader, &r->config, &r->pipeline, p, subpass)); 

    PASS_CALL(CreateStaticBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, verts, sizeof(verts), &r->verts));
    PASS_CALL(CreateStaticBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indicies, sizeof(indicies), &r->index));

    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        PASS_CALL(CreateDynamicBuffer(sizeof(Uniform), &r->uniform[i], VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));
    }

    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        PASS_CALL(SetBuffer(&r->config, SR_DESC_UNIFORM, (Buffer*)&r->uniform[i], 0, i));
    }
    return SR_NO_ERROR;
}


void FractalSetZoom(FractalRenderer* r, float z) {
    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        ((Uniform*)(r->uniform[i].buffer))->zoom = z;
    }
}

void FractalSetColor(FractalRenderer* r, sm_vec3f c) {
    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        ((Uniform*)(r->uniform[i].buffer))->color = c;
    }
}

void FractalSetPos(FractalRenderer* r, sm_vec2f pos) {
    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        ((Uniform*)(r->uniform[i].buffer))->pos = pos;
    }
}

void FractalSetInit(FractalRenderer* r, sm_vec2f init) {
    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        ((Uniform*)(r->uniform[i].buffer))->initial = init;
    }
}

ErrorCode FractalGetSubpass(SubPass* s, Attachment* a, u32 start) {
    *s = (SubPass) {
        .numAttachments = 0,
        .colorAttachment = 0,
        .depthAttachment = -1,
        .firstAttachment = 0,
    };
    return SR_NO_ERROR;
}

void FractalDrawFrame(FractalRenderer* r, PresentInfo* p, u32 frame) {
    ((Uniform*)(r->uniform[frame].buffer))->size = 
        (sm_vec2f){WIDTH, HEIGHT};

    //memcpy(r->uniform[frame].buffer, &ub, sizeof(Uniform));
    
    VkCommandBuffer cmdBuf = sr_context.cmd.buffer[frame];

    vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, r->pipeline.pipeline);

    VkBuffer bufs[] = {r->verts.vhandle};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmdBuf, 0, 1, bufs, offsets);
    vkCmdBindIndexBuffer(cmdBuf, r->index.vhandle, 0, VK_INDEX_TYPE_UINT16); 
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

void FractalDestroy(FractalRenderer* r) {
    vkDeviceWaitIdle(sr_device.l);
    DestroyStaticBuffer(&r->verts);
    DestroyStaticBuffer(&r->index);
    DestroyPipeline(&r->pipeline);
    DestroyShaderProg(&r->shader);
    DestroyPipelineConfig(&r->config);
    VkDevice d = sr_device.l;
    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(d, r->uniform[i].vhandle, NULL);
        vkFreeMemory(d, r->uniform[i].mem, NULL);
    }
}
