#include "sprite.h"
#include "error.h"
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

static const Vertex verts[] = {
    {{-0.5f, -0.5f}, {0.0f, 0.0f}},
    {{ 0.5f, -0.5f}, {1.0f, 0.0f}},
    {{ 0.5f,  0.5f}, {1.0f, 1.0f}},
    {{-0.5f,  0.5f}, {0.0f, 1.0f}},
};

static const uint16_t indicies[] = {
    0, 1, 2, 2, 3, 0
};


ErrorCode SpriteInit(RenderState* r, Camera c, uint textureSlots) {


    PASS_CALL(CreateShaderProg("shaders/standard.vert.spv", "shaders/standard.frag.spv", &r->shader));

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
    PASS_CALL(CreatePipelineConfig(&r->shader, VulkanVertToConfig(vin), &r->config));
    PASS_CALL(CreatePass(&r->pass));
    PASS_CALL(CreateSwapChain(&r->pass, &r->swap, VK_NULL_HANDLE));
    PASS_CALL(CreatePipeline(&r->shader, &r->config, &r->pipeline, &r->pass)); 
    PASS_CALL(CreateCommand(&r->cmd));
    
    PASS_CALL(CreateStaticBuffer(&r->cmd, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, verts, sizeof(verts), &r->verts));
    PASS_CALL(CreateStaticBuffer(&r->cmd, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indicies, sizeof(indicies), &r->index));

    PASS_CALL(SetBuffer(&r->config, SR_DESC_UNIFORM, &r->uniforms, 0));
    PASS_CALL(SetBuffer(&r->config, SR_DESC_STORAGE, &r->modelBuf, 1));

    r->cam = c;
    return SR_NO_ERROR;
}


//set texture to slot
ErrorCode SetTextureSlot(RenderState* r, Texture* t, u32 index) {
    PASS_CALL(SetImage(t->view, t->sampler, &r->config, 2, index));
    return SR_NO_ERROR;
}

//Sets all textures starting from texture id 0
ErrorCode SetTextureSlots(RenderState* r, Texture* t, u32 number) {
    VkImageView views[number];
    VkSampler samplers[number];
    for (int i = 0; i < number; i++) {
        views[i] = t[i].view;
        samplers[i] = t[i].sampler;
    }

    PASS_CALL(SetImages(views, samplers, &r->config, 2, number));
    return SR_NO_ERROR;
}


void SpriteDestroy(RenderState* r) {
    vkDeviceWaitIdle(sr_device.l);
    DestroyStaticBuffer(&r->verts);
    DestroyStaticBuffer(&r->index);
    DestroyCommand(&r->cmd);
    DestroyPipeline(&r->pipeline);
    DestroyShaderProg(&r->shader);
    DestroyPipelineConfig(&r->config);
    DestroyPass(&r->pass);
    VkDevice d = sr_device.l;
    for (size_t i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(d, r->uniforms.bufs[i], NULL);
        vkFreeMemory(d, r->uniforms.mem[i], NULL);
    }
    for (size_t i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(d, r->modelBuf.bufs[i], NULL);
        vkFreeMemory(d, r->modelBuf.mem[i], NULL);
    }
    DestroySwapChain(&r->swap);
}




SpriteHandle CreateSprite(RenderState* r, sm_vec2f pos, sm_vec2f size, u32 tex) {
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
        .texture = tex
    };


    denseSetIdx[denseIndex] = newIndex;

    return newIndex;
}

ErrorCode DestroySprite(RenderState* r, SpriteHandle s) {

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



ErrorCode PushBuffer(RenderState* r, void* buf) {
    SpritePack* packBuf = buf;
    for (int i = 0; i < r->denseSize; i++) {
        SpriteEntry sprite = r->denseSetVals[i];
        sm_mat4f model = SM_MAT4_IDENTITY;        
        model = sm_mat4_f32_scale(&model, (sm_vec4f){sprite.size.x, sprite.size.y, 1.0f, 1.0f});
        model = sm_mat4_f32_rz(&model, sprite.rotation);
        model = sm_mat4_f32_translate(&model, (sm_vec3f){sprite.pos.x, sprite.pos.y, 0.0f}); 

        packBuf[i] = (SpritePack) {
            .model = model,
            .texture = sprite.texture
        };
    }
    return SR_NO_ERROR;
}
SpriteEntry* GetSprite(RenderState* r, SpriteHandle s) {
    if (!r->sparseSet[s]) return NULL;
    return &r->denseSetVals[(r->sparseSet[s] - 1)];
}

Camera* GetCam(RenderState* r) {
    return &r->cam;
}


u32 GetNum(RenderState* r) {
    return r->denseSize;
}



void DrawFrame(RenderState* r, unsigned int frame) {
    VulkanDevice* device = &sr_device;
    VulkanContext* context = &sr_context; 
    VulkanCommand* cmd = &r->cmd;
    VulkanShader* shader = &r->shader;
    VulkanPipelineConfig* config = &r->config;
    VulkanPipeline* pipe = &r->pipeline;
    RenderPass* pass = &r->pass;
    SwapChain* swapchain = &r->swap;
    BufferHandle* uniforms = &r->uniforms;
    

    vkWaitForFences(device->l, 1, &cmd->inFlight[frame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device->l, swapchain->swapChain, UINT64_MAX, cmd->imageAvalible[frame], NULL, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {


        vkDeviceWaitIdle(device->l);
        SwapChain old = *swapchain;

        ErrorCode code = CreateSwapChain(pass, swapchain, swapchain->swapChain); 
        DestroySwapChain(&old);
        if (code != SR_NO_ERROR) SR_LOG_ERR("SwapChain failed to Recreate");
        return;

    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        SR_LOG_ERR("Bad things are happening");
    }

    sm_mat4f view = SM_MAT4_IDENTITY;
    sm_mat4f proj = SM_MAT4_IDENTITY;

    float aspect = ((float)WIDTH)/((float)HEIGHT);
    proj = sm_mat4_f32_ortho(1.0f, 3.0f, -aspect, aspect, -1.0f, 1.0f);

    Camera cam = r->cam;
    view = sm_mat4_f32_scale(&view, (sm_vec4f){1/cam.size.x, 1/cam.size.y, 1.0f, 1.0f});
    view = sm_mat4_f32_rz(&view, -cam.rotation);
    view = sm_mat4_f32_translate(&view, (sm_vec3f){-cam.pos.x, -cam.pos.y, 0.0f});


    memcpy(uniforms->objs[frame], &view, sizeof(sm_mat4f));
    memcpy(uniforms->objs[frame] + sizeof(sm_mat4f), &proj, sizeof(sm_mat4f));
    PushBuffer(r, r->modelBuf.objs[frame]);

    vkResetFences(device->l, 1, &cmd->inFlight[frame]);
    vkResetCommandBuffer(cmd->buffer[frame], 0);
    VkCommandBuffer cmdBuf = cmd->buffer[frame];
    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    if (vkBeginCommandBuffer(cmdBuf, &beginInfo) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Begin Command Buffer");
        return;
    }
    
    VkRenderPassBeginInfo renderInfo = {0};
    renderInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderInfo.renderPass = pass->pass;
    renderInfo.framebuffer = swapchain->buffers[imageIndex];
    renderInfo.renderArea.offset = (VkOffset2D){0, 0};
    renderInfo.renderArea.extent = swapchain->extent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderInfo.clearValueCount = 1;
    renderInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(cmdBuf, &renderInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipe->pipeline);

    VkBuffer bufs[] = {r->verts.buf};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmdBuf, 0, 1, bufs, offsets);
    vkCmdBindIndexBuffer(cmdBuf, r->index.buf, 0, VK_INDEX_TYPE_UINT16); 

    
    pipe->view.x = 0.0f;
    pipe->view.y = 0.0f;
    pipe->view.width = swapchain->extent.width;
    pipe->view.height = swapchain->extent.height;
    pipe->view.minDepth = 0.0f;
    pipe->view.maxDepth = 1.0f;
    vkCmdSetViewport(cmdBuf, 0, 1, &pipe->view);

    pipe->scissor.offset = (VkOffset2D){0, 0};
    pipe->scissor.extent = swapchain->extent;
    vkCmdSetScissor(cmdBuf, 0, 1, &pipe->scissor);

    vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, config->layout, 0, 1, &config->descrip.descriptorSet[frame], 0, NULL);

    vkCmdDrawIndexed(cmdBuf, r->index.size/sizeof(uint16_t), r->denseSize, 0, 0, 0);
    vkCmdEndRenderPass(cmdBuf);


    if (vkEndCommandBuffer(cmdBuf) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to End Command Buffer");
        return;
    }

    //update uniforms



    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {cmd->imageAvalible[frame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd->buffer[frame];

    VkSemaphore signalSemaphores[] = {cmd->renderFinished[frame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(device->graph, 1, &submitInfo, cmd->inFlight[frame]) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Submit Queue");
        return;
    }

    VkPresentInfoKHR presentInfo = {0};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapchain->swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(device->present, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR ||
        result == VK_SUBOPTIMAL_KHR ||
        frameBufferResized) {

        frameBufferResized = FALSE;
        vkDeviceWaitIdle(device->l);
        DestroySwapChain(swapchain);

        ErrorCode code = CreateSwapChain(pass, swapchain, NULL);
        if (code != SR_NO_ERROR) SR_LOG_ERR("SwapChain failed to Recreate");
        return;

    } else if (result != VK_SUCCESS) {
        SR_LOG_ERR("Bad things are happening");
    }
}
