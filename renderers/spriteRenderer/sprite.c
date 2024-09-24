#include "sprite.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include <string.h>
#include <mat4.h>
#include <vec2.h>
#include <vulkan/vulkan_core.h>
#include "common.h"
#include "config.h"
#include "memory.h"
#include "pipeline.h"
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


ErrorCode SpriteInit(RenderState* r) {


    PASS_CALL(CreateShaderProg("shaders/standard.vert.spv", "shaders/standard.frag.spv", &r->shader));

    DescriptorType uniformconfig[2] = {SR_DESC_UNIFORM, SR_DESC_SAMPLER};
    DescriptorDetail flags[2] = {{VK_SHADER_STAGE_VERTEX_BIT, 0}, {VK_SHADER_STAGE_FRAGMENT_BIT, 2}};
    PASS_CALL(CreateDescriptorSetConfig(&r->config, uniformconfig, flags, 2));


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


    PASS_CALL(CreateImage(&r->cmd, &r->test1, "resources/textures/texture.jpg"));
    PASS_CALL(CreateImage(&r->cmd, &r->test2, "resources/textures/duck.jpg"));

    PASS_CALL(SetImage(r->test1.view, r->test1.sampler, &r->config, 1, 0));
    PASS_CALL(SetImage(r->test2.view, r->test2.sampler, &r->config, 1, 1));
    PASS_CALL(SetBuffer(&r->config, &r->uniforms, 0));

    return SR_NO_ERROR;
}

void SpriteDestroy(RenderState* r) {
    vkDeviceWaitIdle(sr_device.l);
    DestroyImage(&r->test1);
    DestroyImage(&r->test2);
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
    DestroySwapChain(&r->swap);
}




SpriteHandle CreateSprite(RenderState* r, sm_vec3f pos, sm_vec3f size) {
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

    sm_vec4f s = (sm_vec4f){size.x, size.y, size.z, 1.0f};

    denseSetVals[denseIndex].model = sm_mat4_f32_scale(&sm_mat4f_identity, s);
    denseSetVals[denseIndex].model = sm_mat4_f32_translate(&denseSetVals[denseIndex].model, pos); 

    denseSetVals[denseIndex].texture = 0;


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
    memcpy(buf, r->denseSetVals, sizeof(SpriteEntry) * r->denseSize);
    return SR_NO_ERROR;
}
sm_mat4f* GetModel(RenderState* r, SpriteHandle s) {
    if (!r->sparseSet[s]) return NULL;
    return &r->denseSetVals[(r->sparseSet[s] - 1)].model;
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
        DestroySwapChain(swapchain);

        ErrorCode code = CreateSwapChain(pass, swapchain, swapchain->swapChain); 
        if (code != SR_NO_ERROR) return;


    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        SR_LOG_ERR("Bad things are happening");
    }

    sm_mat4f view = SM_MAT4_IDENTITY;
    sm_mat4f proj = SM_MAT4_IDENTITY;


    proj = sm_mat4_f32_perspective(1.0f, 1000.0f, SM_PI/4, ((float)WIDTH)/((float)HEIGHT));
    view = sm_mat4_f32_translate(&view, (sm_vec3f){-50, -50, 50});
    //proj = sm_mat4_f32_ortho(1.0f, 100.0f, 0.0f, 100.0f, 0.0f, 100.0f);


    memcpy(uniforms->objs[frame], &view, sizeof(sm_mat4f));
    memcpy(uniforms->objs[frame] + sizeof(sm_mat4f), &proj, sizeof(sm_mat4f));
    PushBuffer(r, uniforms->objs[frame] + sizeof(sm_mat4f) * 2);

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
        if (code != SR_NO_ERROR) return;

    } else if (result != VK_SUCCESS) {
        SR_LOG_ERR("Bad things are happening");
    }
}
