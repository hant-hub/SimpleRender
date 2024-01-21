#include "Vulkan.h"
#include "error.h"
#include "util.h"
#include "instance.h"
#include "optional.h"
#include "physicaldevice.h"
#include "logicaldevice.h"
#include "swapchain.h"
#include "imageviews.h"
#include "lib/SimpleMath/src/include/misc.h"
#include "pipeline.h"
#include "framebuffer.h"
#include "command.h"
#include "state.h"
#include "init.h"
#include <GLFW/glfw3.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>



static const uint32_t WIDTH = 1920;
static const uint32_t HEIGHT = 1080;







inline static void CloseGLFW(GLFWwindow *w) {
    glfwDestroyWindow(w);
    glfwTerminate();
}

int main() {

    VulkanState state = {0};
    glfwInit();
    //create Window
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    state.w = glfwCreateWindow(WIDTH, HEIGHT, "Renderer", glfwGetPrimaryMonitor(), NULL);

    errno = NoError;
    InitVulkan(&state);
    if (errno != NoError)
        exit(EXIT_FAILURE);
    


    //Sync Objects
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    VkSemaphoreCreateInfo semaphoreInfo = {0};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {0};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(state.logical.device, &semaphoreInfo, NULL, &imageAvailableSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(state.logical.device, &semaphoreInfo, NULL, &renderFinishedSemaphore) != VK_SUCCESS ||
            vkCreateFence(state.logical.device, &fenceInfo, NULL, &inFlightFence) != VK_SUCCESS) {
        ExitVulkan(&state);
        exit(EXIT_FAILURE);

    }

    

    while (!glfwWindowShouldClose(state.w)){
        glfwPollEvents();

        //Draw Frame
        vkWaitForFences(state.logical.device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(state.logical.device, 1, &inFlightFence); 

        uint32_t imageIndex;
        vkAcquireNextImageKHR(state.logical.device, state.swapData.swapchains[0], UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

        vkResetCommandBuffer(state.command.commandbuffers[0], 0);
        RecordCommands(&state.command.commandbuffers[0], 
                state.pipe.renderpasses[0], &state.pipe.pipeline, &state.swapData, state.swapData.frameBuffers, imageIndex);

        VkSubmitInfo submitInfo = {0};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = state.command.commandbuffers;

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(state.logical.graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
            ExitVulkan(&state);
            exit(EXIT_FAILURE);
        }

        VkPresentInfoKHR presentInfo = {0};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        presentInfo.swapchainCount = state.swapData.swapCount;
        presentInfo.pSwapchains = state.swapData.swapchains;
        presentInfo.pImageIndices = &imageIndex;

        presentInfo.pResults = NULL;

        vkQueuePresentKHR(state.logical.graphicsQueue, &presentInfo);



    }

    vkQueueWaitIdle(state.logical.graphicsQueue);
    vkDeviceWaitIdle(state.logical.device);
    vkDestroySemaphore(state.logical.device, imageAvailableSemaphore, NULL);
    vkDestroySemaphore(state.logical.device, renderFinishedSemaphore, NULL);
    vkDestroyFence(state.logical.device, inFlightFence, NULL);
    ExitVulkan(&state);

    return EXIT_SUCCESS;
}
