#include "frame.h"
#include "sprite.h"




void DrawFrame(VulkanDevice* device, VulkanCommand* cmd, GeometryBuffer* buffer, VulkanContext* context, VulkanShader* s,
                      VulkanPipelineConfig* config, SwapChain* swapchain, VulkanPipeline* pipe, UniformHandles* uniforms, unsigned int frame) {

    vkWaitForFences(device->l, 1, &cmd->inFlight[frame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device->l, swapchain->swapChain, UINT64_MAX, cmd->imageAvalible[frame], NULL, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {

        vkDeviceWaitIdle(device->l);
        DestroySwapChain(device->l, swapchain);

        ErrorCode code = CreateSwapChain(device, context, swapchain, NULL);
        if (code != SR_NO_ERROR) return;
        code = CreateFrameBuffers(device, swapchain, config);
        if (code != SR_NO_ERROR) return;


    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        SR_LOG_ERR("Bad things are happening");
    }

    static sm_mat4f model = {
        {1.0f, 0, 0, 0},
        {0, 1.0f, 0, 0},
        {0, 0, 1.0f, 0},
        {0, 0, 0, 1.0f}
    };

    static sm_mat4f model2 = {
        {1.0f, 0, 0, 0},
        {0, 1.0f, 0, 0},
        {0, 0, 1.0f, 0},
        {0, 0, 0, 1.0f}
    };

    sm_mat4f scaler = {
        {1.0f, 0, 0, 0},
        {0, 1.0f, 0, 0},
        {0, 0, 1.0f, 0},
        {0, 0, 0, 1.0f}
    };

    sm_vec3f mov = (sm_vec3f) {0.01f, 0, 0};

    PushBuffer(uniforms->objs[frame]);

    vkResetFences(device->l, 1, &cmd->inFlight[frame]);
    vkResetCommandBuffer(cmd->buffer[frame], 0);
    RecordCommandBuffer(swapchain, pipe, config, &cmd->buffer[frame], buffer, imageIndex, frame, GetNum());

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

        ErrorCode code = CreateSwapChain(device, context, swapchain, NULL);
        if (code != SR_NO_ERROR) return;
        code = CreateFrameBuffers(device, swapchain, config);
        if (code != SR_NO_ERROR) return;

    } else if (result != VK_SUCCESS) {
        SR_LOG_ERR("Bad things are happening");
    }
}
