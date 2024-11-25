#include "frame.h"
#include "config.h"
#include "error.h"
#include "init.h"
#include "pipeline.h"
#include <vulkan/vulkan_core.h>





ErrorCode InitPresent(PresentInfo* p, RenderPass* r) {
    PASS_CALL(CreateSwapChain(r, &p->swapchain, NULL));

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
    p->p = r;
    return SR_NO_ERROR;
}

void NextPass(PresentInfo* p) {
    vkCmdNextSubpass(sr_context.cmd.buffer[p->imageIndex], VK_SUBPASS_CONTENTS_INLINE);
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
        ErrorCode code = CreateSwapChain(p->p, &p->swapchain, NULL); 
        if (code != SR_NO_ERROR) SR_LOG_ERR("SwapChain failed to Recreate");
        return SR_OP_FAIL;

    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        SR_LOG_ERR("Bad things are happening");
    }
    return SR_NO_ERROR;
}


ErrorCode SubmitFrame(PresentInfo* p, u32 frame) {
    VkSemaphore signalSemaphores[] = {p->renderFinished[frame]};
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
        DestroySwapChain(&p->swapchain, p->p);
        ErrorCode code = CreateSwapChain(p->p, swapchain, NULL);
        vkDeviceWaitIdle(sr_device.l);

        if (code != SR_NO_ERROR) SR_LOG_ERR("SwapChain failed to Recreate");
        return SR_OP_FAIL;

    } else if (result != VK_SUCCESS) {
        SR_LOG_ERR("Bad things are happening");
    }
    return SR_NO_ERROR;
}

void DestroyPresent(PresentInfo* p) {
    DestroySwapChain(&p->swapchain, p->p);
    VkDevice d = sr_device.l;
    for (unsigned int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyFence(d, p->inFlight[i], NULL);
        vkDestroySemaphore(d, p->renderFinished[i], NULL);
        vkDestroySemaphore(d, p->imageAvalible[i], NULL);
    }
    SR_LOG_DEB("Destroyed Sync Objects");
}
