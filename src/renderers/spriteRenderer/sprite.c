#include "sprite.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include <string.h>
#include <mat4.h>
#include <vulkan/vulkan_core.h>
#include "common.h"
#include "config.h"
#include "pipeline.h"



typedef struct {
    sm_mat4f model;
//    i32 texture;
} SpriteEntry;


static SpriteEntry denseSetVals[SR_MAX_INSTANCES] = {0};
static u32 denseSetIdx[SR_MAX_INSTANCES] = {0};
static i32 sparseSet[SR_MAX_INSTANCES] = {0};
static u32 denseSize = 0;


SpriteHandle CreateSprite(sm_vec3f pos, sm_vec3f size) {
    //sparse set is index + 1,
    //all valid handles must be >0, since 0 is
    //a tombstone value
    
    
    u32 newIndex = 0;
    while (newIndex < SR_MAX_INSTANCES && sparseSet[newIndex++] != 0);
    if (newIndex >= SR_MAX_INSTANCES) {
        SR_LOG_ERR("Failed to Create Sprite! Not Enough Instances");
        return -1;
    }
    u32 denseIndex = denseSize;
    sparseSet[--newIndex] = ++denseSize; 

    sm_vec4f s = (sm_vec4f){size.x, size.y, size.z, 1.0f};

    denseSetVals[denseIndex].model = sm_mat4_f32_scale(&sm_mat4f_identity, s);
    denseSetVals[denseIndex].model = sm_mat4_f32_translate(&denseSetVals[denseIndex].model, pos); 


    denseSetIdx[denseIndex] = newIndex;

    return newIndex;
}

ErrorCode DestroySprite(SpriteHandle s) {

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

    denseSize -= 1;
    sparseSet[s] = 0;

    return SR_NO_ERROR;
}



ErrorCode PushBuffer(void* buf) {

    memcpy(buf, denseSetVals, sizeof(SpriteEntry) * denseSize);
    return SR_NO_ERROR;
}
sm_mat4f* GetModel(SpriteHandle s) {
    if (!sparseSet[s]) return NULL;
    return &denseSetVals[(sparseSet[s] - 1)].model;
}



u32 GetNum() {
    return denseSize;
}







void DrawFrame(RenderState r, unsigned int frame) {
    VulkanDevice* device = r.d;
    VulkanContext* context = r.context; 
    VulkanCommand* cmd = r.cmd;
    VulkanShader* shader = r.shader;
    VulkanPipelineConfig* config = r.config;
    VulkanPipeline* pipe = r.pipeline;
    RenderPass* pass = r.pass;
    SwapChain* swapchain = r.swap;
    UniformHandles* uniforms = r.uniforms;
    

    vkWaitForFences(device->l, 1, &cmd->inFlight[frame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device->l, swapchain->swapChain, UINT64_MAX, cmd->imageAvalible[frame], NULL, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {

        vkDeviceWaitIdle(device->l);
        DestroySwapChain(device->l, swapchain);

        ErrorCode code = CreateSwapChain(device, context, pass, swapchain, swapchain->swapChain); 
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
    PushBuffer(uniforms->objs[frame] + sizeof(sm_mat4f) * 2);

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

    VkBuffer bufs[] = {r.verts->buf};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmdBuf, 0, 1, bufs, offsets);
    vkCmdBindIndexBuffer(cmdBuf, r.index->buf, 0, VK_INDEX_TYPE_UINT16); 

    
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

    vkCmdDrawIndexed(cmdBuf, r.index->size/sizeof(uint16_t), GetNum(), 0, 0, 0);
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
        DestroySwapChain(device->l, swapchain);

        ErrorCode code = CreateSwapChain(device, context, pass, swapchain, NULL);
        if (code != SR_NO_ERROR) return;

    } else if (result != VK_SUCCESS) {
        SR_LOG_ERR("Bad things are happening");
    }
}
