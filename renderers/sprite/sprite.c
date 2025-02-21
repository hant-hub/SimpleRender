#include "sprite.h"
#include "error.h"
#include "frame.h"
#include "init.h"
#include "log.h"
#include <string.h>
#include <mat4.h>
#include <strings.h>
#include <vec2.h>
#include <vulkan/vulkan_core.h>
#include "common.h"
#include "config.h"
#include "memory.h"
#include "pipeline.h"
#include "texture.h"
#include "util.h"

typedef struct {
    sm_vec2f pos;
    sm_vec2f uv;
} Vertex;

typedef struct {
    sm_mat4f model;
    sm_mat4f view;
    sm_mat4f proj;
} UniformObj;

static const Vertex verts[] = {
    {{-0.5f, -0.5f}, {0.0f, 0.0f}},
    {{ 0.5f, -0.5f}, {1.0f, 0.0f}},
    {{ 0.5f,  0.5f}, {1.0f, 1.0f}},
    {{-0.5f,  0.5f}, {0.0f, 1.0f}},
};

static const uint16_t indicies[] = {
    0, 1, 2, 2, 3, 0
};



ErrorCode SpriteInit(SpriteRenderer* r, RenderPass* p, u32 subpass, Camera c, u32 textureSlots) {

    PASS_CALL(CreateShaderProg("shaders/sprite/sprite.vert.spv", "shaders/sprite/sprite.frag.spv", &r->shader));

    DescriptorDetail descriptorConfigs[] = {
        {SR_DESC_UNIFORM, VK_SHADER_STAGE_VERTEX_BIT,   0}, 
        {SR_DESC_STORAGE, VK_SHADER_STAGE_VERTEX_BIT, 0},
        {SR_DESC_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, textureSlots}
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
    PASS_CALL(CreateStaticBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indicies, sizeof(indicies), &r->index));

    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        PASS_CALL(CreateDynamicBuffer(2 * sizeof(sm_mat4f), &r->uniforms[i], VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));
        PASS_CALL(CreateDynamicBuffer(SR_MAX_INSTANCES * sizeof(SpritePack), &r->modelBuf[i], VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
    }

    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        PASS_CALL(SetBuffer(&r->config, SR_DESC_UNIFORM, (Buffer*)&r->uniforms[i], 0, i));
        PASS_CALL(SetBuffer(&r->config, SR_DESC_STORAGE, (Buffer*)&r->modelBuf[i], 1, i));
    }

    r->cam = c;
    return SR_NO_ERROR;
}

ErrorCode SpriteGetSubpass(SubPass* s, Attachment* a, u32 start) {
    *s = (SubPass) {
        .numAttachments = SR_SPRITE_ATTACHMENT_NUM,
        .colorAttachment = 0,
        .depthAttachment = start,
        .firstAttachment = start,
    };
    a[start] = (Attachment) {
        .type = SR_ATTATCHMENT_DEPTH,
    };

    return SR_NO_ERROR;
}


//set texture to slot
ErrorCode SetTextureSlot(SpriteRenderer* r, Texture* t, u32 index) {
    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        PASS_CALL(SetImage(t->image.view, t->sampler, &r->config, 2, index, i));
    }
    return SR_NO_ERROR;
}

//Sets all textures starting from texture id 0
ErrorCode SetTextureSlots(SpriteRenderer* r, Texture* t, u32 number) {
    VkImageView views[number];
    VkSampler samplers[number];
    for (int i = 0; i < number; i++) {
        views[i] = t[i].image.view;
        samplers[i] = t[i].sampler;
    }

    PASS_CALL(SetImages(views, samplers, &r->config, 2, number));
    return SR_NO_ERROR;
}


void SpriteDestroy(SpriteRenderer* r) {
    vkDeviceWaitIdle(sr_device.l);
    DestroyStaticBuffer(&r->verts);
    DestroyStaticBuffer(&r->index);
//    DestroyCommand(&sr_context.cmd);
    DestroyPipeline(&r->pipeline);
    DestroyShaderProg(&r->shader);
    DestroyPipelineConfig(&r->config);
    //DestroyPass(&r->pass);
    VkDevice d = sr_device.l;
    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(d, r->uniforms[i].vhandle, NULL);
        vkFreeMemory(d, r->uniforms[i].mem, NULL);
        vkDestroyBuffer(d, r->modelBuf[i].vhandle, NULL);
        vkFreeMemory(d, r->modelBuf[i].mem, NULL);
    }
}




SpriteHandle CreateSprite(SpriteRenderer* r, sm_vec2f pos, sm_vec2f size, u32 tex, u32 layer) {
    //sparse set is index + 1,
    //all valid handles must be >0, since 0 is
    //a tombstone value
    
    SpriteEntry* denseSetVals = r->denseSetVals;
    u32* denseSetIdx = r->denseSetIdx;
    i32* sparseSet = r->sparseSet;
    u32* denseSize = &r->denseSize;
    
    
    u32 newIndex = 0;
    while (newIndex < SR_MAX_INSTANCES && sparseSet[newIndex++] != 0);
    if (newIndex >= SR_MAX_INSTANCES) {
        SR_LOG_ERR("Failed to Create Sprite! Not Enough Instances");
        return -1;
    }
    u32 denseIndex = *denseSize;
    sparseSet[--newIndex] = ++*denseSize; 

    sm_vec4f s = (sm_vec4f){size.x, size.y, 1.0f, 1.0f};

    denseSetVals[denseIndex] = (SpriteEntry) {
        .pos = pos,
        .size = size,
        .rotation = 0,
        .layer = layer,
        .texture = tex
    };


    denseSetIdx[denseIndex] = newIndex;

    return newIndex;
}

ErrorCode DestroySprite(SpriteRenderer* r, SpriteHandle s) {

    SpriteEntry* denseSetVals = r->denseSetVals;
    u32* denseSetIdx = r->denseSetIdx;
    i32* sparseSet = r->sparseSet;
    u32 denseSize = r->denseSize;

    u32 denseIndex = sparseSet[s] - 1;
    {
        SpriteEntry temp = denseSetVals[denseSize - 1];
        denseSetVals[denseSize - 1] = denseSetVals[denseIndex];
        denseSetVals[denseIndex] = temp;
    }

    {
        u32 temp = denseSetIdx[denseSize - 1];
        denseSetIdx[denseSize - 1] = denseSetIdx[denseIndex];
        denseSetIdx[denseIndex] = temp;
    }

    r->denseSize -= 1;
    sparseSet[s] = 0;

    return SR_NO_ERROR;
}



ErrorCode PushBuffer(SpriteRenderer* r, void* buf) {
    SpritePack* packBuf = buf;
    for (int i = 0; i < r->denseSize; i++) {
        SpriteEntry sprite = r->denseSetVals[i];
        sm_mat4f model = SM_MAT4_IDENTITY;        
        model = sm_mat4_f32_scale(&model, (sm_vec4f){sprite.size.x, sprite.size.y, 1.0f, 1.0f});
        model = sm_mat4_f32_rz(&model, sprite.rotation);
        model = sm_mat4_f32_translate(&model, (sm_vec3f){sprite.pos.x, sprite.pos.y, sprite.layer}); 

        packBuf[i] = (SpritePack) {
            .model = model,
            .texture = sprite.texture
        };
    }
    return SR_NO_ERROR;
}
SpriteEntry* GetSprite(SpriteRenderer* r, SpriteHandle s) {
    if (!r->sparseSet[s]) return NULL;
    return &r->denseSetVals[(r->sparseSet[s] - 1)];
}

Camera* GetCam(SpriteRenderer* r) {
    return &r->cam;
}


u32 GetNum(SpriteRenderer* r) {
    return r->denseSize;
}

void SpriteDrawFrame(SpriteRenderer* r, PresentInfo* p, unsigned int frame) {
    VulkanDevice* device = &sr_device;
    VulkanContext* context = &sr_context; 
    VulkanCommand* cmd = &sr_context.cmd;
    VulkanShader* shader = &r->shader;
    VulkanPipelineConfig* config = &r->config;
    VulkanPipeline* pipe = &r->pipeline;
    DynamicBuffer* uniforms = &r->uniforms[frame];
    
    sm_mat4f view = SM_MAT4_IDENTITY;
    sm_mat4f proj = SM_MAT4_IDENTITY;

    float aspect = ((float)fWIDTH)/((float)fHEIGHT);
    proj = sm_mat4_f32_ortho(1.0f, (float)MAX_LAYERS + 1.0f, -aspect, aspect, -1.0f, 1.0f);

    Camera cam = r->cam;
    view = sm_mat4_f32_scale(&view, (sm_vec4f){1/cam.size.x, 1/cam.size.y, 1.0f, 1.0f});
    view = sm_mat4_f32_rz(&view, -cam.rotation);
    view = sm_mat4_f32_translate(&view, (sm_vec3f){-cam.pos.x, -cam.pos.y, 0.0f});


    memcpy(uniforms->buffer, &view, sizeof(sm_mat4f));
    memcpy(uniforms->buffer + sizeof(sm_mat4f), &proj, sizeof(sm_mat4f));
    PushBuffer(r, r->modelBuf[frame].buffer);

    VkCommandBuffer cmdBuf = cmd->buffer[frame];

    vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipe->pipeline);

    VkBuffer bufs[] = {r->verts.vhandle};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmdBuf, 0, 1, bufs, offsets);
    vkCmdBindIndexBuffer(cmdBuf, r->index.vhandle, 0, VK_INDEX_TYPE_UINT16); 

    
    pipe->view.x = 0.0f;
    pipe->view.y = 0.0f;
    pipe->view.width = p->swapchain.extent.width;
    pipe->view.height = p->swapchain.extent.height;
    pipe->view.minDepth = 0.0f;
    pipe->view.maxDepth = 1.0f;
    vkCmdSetViewport(cmdBuf, 0, 1, &pipe->view);

    pipe->scissor.offset = (VkOffset2D){0, 0};
    pipe->scissor.extent = p->swapchain.extent;
    vkCmdSetScissor(cmdBuf, 0, 1, &pipe->scissor);

    vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, config->layout, 0, 1, &config->descrip.descriptorSet[frame], 0, NULL);

    vkCmdDrawIndexed(cmdBuf, r->index.size/sizeof(uint16_t), r->denseSize, 0, 0, 0);
}
