#include "frame.h"



ErrorCode DrawFrame(VulkanDevice* d, SwapChain* s, Command* c, Pipeline* p,
                    SyncObjects* sync, int* framebufferResized, uint32_t currentFrame) {

        glfwPollEvents();

        //Draw Frame
        vkWaitForFences(d->logical.device, 1, &sync->fences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(d->logical.device, s->swapchain, UINT64_MAX, sync->imageAvalible[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR){
            vkDeviceWaitIdle(d->logical.device);
            
            DestroyPipeline(d, s, p, FRAMEBUFFER);
            DestroySwapChain(d, s, IMAGEVIEWS);

            CreateSwapChain(d, s, COMPLETE);
            CreatePipeline(d, s, p, FRAMEBUFFER);
            return NoError;
        } else if (result != VK_SUCCESS) {
            return Error;
        }

        vkResetFences(d->logical.device, 1, &sync->fences[currentFrame]); 

        vkResetCommandBuffer(c->buffers[currentFrame], 0);
        RecordCommands(&c->buffers[currentFrame], p, s, imageIndex);

        VkSubmitInfo submitInfo = {0};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {sync->imageAvalible[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &c->buffers[currentFrame];

        VkSemaphore signalSemaphores[] = {sync->renderFinished[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(d->logical.graphicsQueue, 1, &submitInfo, sync->fences[currentFrame]) != VK_SUCCESS) {
            return Error;
        }

        VkPresentInfoKHR presentInfo = {0};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &s->swapchain;
        presentInfo.pImageIndices = &imageIndex;

        presentInfo.pResults = NULL;

        result = vkQueuePresentKHR(d->logical.graphicsQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || *framebufferResized){
            *framebufferResized = 0;
            int width = 0, height = 0;
            glfwGetFramebufferSize(d->w, &width, &height);
            while (width == 0 || height == 0) {
                printf("hit\n");
                glfwGetFramebufferSize(d->w, &width, &height);
                glfwWaitEvents();
            }

            vkDeviceWaitIdle(d->logical.device);
            
            DestroyPipeline(d, s, p, FRAMEBUFFER);
            DestroySwapChain(d, s, IMAGEVIEWS);

            CreateSwapChain(d, s, COMPLETE);
            CreatePipeline(d, s, p, FRAMEBUFFER);
        } else if (result != VK_SUCCESS) {
            return Error;
        }
    return NoError;
}
