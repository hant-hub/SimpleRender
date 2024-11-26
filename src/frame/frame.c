#include "frame.h"
#include "config.h"
#include "error.h"
#include "init.h"
#include "pipeline.h"
#include "util.h"
#include <vulkan/vulkan_core.h>





ErrorCode InitPresent(PresentInfo* p, SubPass* passes, u32 numPasses, Attachment* attachmens, u32 numAttachments) {

    PASS_CALL(CreateMultiPass(&p->p, passes, numPasses, attachmens, numAttachments));

    PASS_CALL(CreateSwapChain(&p->p, &p->swapchain, NULL));

    VkFenceCreateInfo fenceInfo = {0};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkSemaphoreCreateInfo semaphoreInfo = {0};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (unsigned int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        if (
                vkCreateFence(sr_device.l, &fenceInfo, NULL, &p->inFlight[i])               != VK_SUCCESS ||
                vkCreateSemaphore(sr_device.l, &semaphoreInfo, NULL, &p->imageAvalible[i])  != VK_SUCCESS ||
                vkCreateSemaphore(sr_device.l, &semaphoreInfo, NULL, &p->renderFinished[i]) != VK_SUCCESS
           ) {
            SR_LOG_ERR("Failed to Create all Sync Objects");
            return SR_CREATE_FAIL;
        }
    }
    SR_LOG_DEB("Sync Objects Created");
    return SR_NO_ERROR;
}

void NextPass(PresentInfo* p, u32 frame) {
    vkCmdNextSubpass(sr_context.cmd.buffer[frame], VK_SUBPASS_CONTENTS_INLINE);
}

ErrorCode StartFrame(PresentInfo* p, u32 frame) {
    vkWaitForFences(sr_device.l, 1, &p->inFlight[frame], VK_TRUE, UINT64_MAX);
    VkResult result = vkAcquireNextImageKHR(
            sr_device.l, 
            p->swapchain.swapChain, 
            UINT64_MAX, 
            p->imageAvalible[frame], 
            NULL, 
            &p->imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        vkDeviceWaitIdle(sr_device.l);

        DestroySwapChain(&p->swapchain, p->swapchain.rpass);
        ErrorCode code = CreateSwapChain(&p->p, &p->swapchain, NULL); 
        if (code != SR_NO_ERROR) SR_LOG_ERR("SwapChain failed to Recreate");
        return SR_OP_FAIL;

    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        SR_LOG_ERR("Bad things are happening");
    }
    vkResetFences(sr_device.l, 1, &p->inFlight[frame]);
    vkResetCommandBuffer(sr_context.cmd.buffer[frame], 0);
    VkCommandBuffer cmdBuf = sr_context.cmd.buffer[frame];
    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    if (vkBeginCommandBuffer(cmdBuf, &beginInfo) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Begin Command Buffer");
        return SR_OP_FAIL;
    }

    VkRenderPassBeginInfo renderInfo = {0};
    renderInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderInfo.renderPass = p->p.pass;
    renderInfo.framebuffer = p->swapchain.buffers[p->imageIndex];
    renderInfo.renderArea.offset = (VkOffset2D){0, 0};
    renderInfo.renderArea.extent = p->swapchain.extent;

    VkClearValue clearcolors[3] = {0}; 
    clearcolors[0].color = (VkClearColorValue){0.0f, 0.0f, 0.0f, 1.0f};
    clearcolors[1].depthStencil = (VkClearDepthStencilValue){1.0f, 0}; 
    clearcolors[2].depthStencil = (VkClearDepthStencilValue){1.0f, 0}; 
    renderInfo.clearValueCount = 3;
    renderInfo.pClearValues = clearcolors;

    vkCmdBeginRenderPass(cmdBuf, &renderInfo, VK_SUBPASS_CONTENTS_INLINE);

    return SR_NO_ERROR;
}


ErrorCode SubmitFrame(PresentInfo* p, u32 frame) {
    vkCmdEndRenderPass(sr_context.cmd.buffer[frame]);
    if (vkEndCommandBuffer(sr_context.cmd.buffer[frame]) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to End Command Buffer");
        return SR_OP_FAIL;
    }

    //update uniforms
    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {p->imageAvalible[frame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &sr_context.cmd.buffer[frame];

    VkSemaphore signalSemaphores[] = {p->renderFinished[frame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(sr_device.graph, 1, &submitInfo, p->inFlight[frame]) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Submit Queue");
        return SR_OP_FAIL;
    }

    SwapChain* swapchain = &p->swapchain;
    u32 imageIndex = p->imageIndex;

    VkPresentInfoKHR presentInfo = {0};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapchain->swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    VkResult result = vkQueuePresentKHR(sr_device.present, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR ||
        result == VK_SUBOPTIMAL_KHR ||
        frameBufferResized) {
        vkDeviceWaitIdle(sr_device.l);

        frameBufferResized = FALSE;
        DestroySwapChain(&p->swapchain, &p->p);
        ErrorCode code = CreateSwapChain(&p->p, swapchain, NULL);
        vkDeviceWaitIdle(sr_device.l);

        if (code != SR_NO_ERROR) SR_LOG_ERR("SwapChain failed to Recreate");
        return SR_OP_FAIL;

    } else if (result != VK_SUCCESS) {
        SR_LOG_ERR("Bad things are happening");
    }
    return SR_NO_ERROR;
}

void DestroyPresent(PresentInfo* p) {
    DestroySwapChain(&p->swapchain, &p->p);
    VkDevice d = sr_device.l;
    for (unsigned int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyFence(d, p->inFlight[i], NULL);
        vkDestroySemaphore(d, p->renderFinished[i], NULL);
        vkDestroySemaphore(d, p->imageAvalible[i], NULL);
    }
    SR_LOG_DEB("Destroyed Sync Objects");
    DestroyPass(&p->p);
}
