#include "Vulkan.h"
#include "error.h"
#include "util.h"
#include "instance.h"
#include "optional.h"
#include "physicaldevice.h"
#include "logicaldevice.h"
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

    VulkanDevice d = {0};
    glfwInit();
    //create Window
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    d.w = glfwCreateWindow(WIDTH, HEIGHT, "Renderer", glfwGetPrimaryMonitor(), NULL);

    ErrorCode result = InitVulkan(&d);
    if (result == Error) {
        exit(EXIT_FAILURE);
    }

    SwapChain s = {0};
    result = CreateSwapChain(&d, &s); 
    if (result == Error) {
        ExitVulkan(&d, COMPLETE);
        exit(EXIT_FAILURE);
    }

    Pipeline p = {0};
    result = CreatePipeline(&d, &s, &p);
    if (result == Error) {
        DestroySwapChain(&d, &s, COMPLETE);
        ExitVulkan(&d, COMPLETE);
        exit(EXIT_FAILURE);
    }
    
    Command c = {0};
    result = CreateCommandObjects(&c, &d);
    if (result == Error) {
        DestroyPipeline(&d, &s, &p, COMPLETE);
        DestroySwapChain(&d, &s, COMPLETE);
        ExitVulkan(&d, COMPLETE);
        exit(EXIT_FAILURE);
    }
    


    //Sync Objects
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    VkSemaphoreCreateInfo semaphoreInfo = {0};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {0};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(d.logical.device, &semaphoreInfo, NULL, &imageAvailableSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(d.logical.device, &semaphoreInfo, NULL, &renderFinishedSemaphore) != VK_SUCCESS ||
            vkCreateFence(d.logical.device, &fenceInfo, NULL, &inFlightFence) != VK_SUCCESS) {
        vkDestroyCommandPool(d.logical.device, c.pool, NULL);
        DestroyPipeline(&d, &s, &p, COMPLETE);
        DestroySwapChain(&d, &s, COMPLETE);
        ExitVulkan(&d, COMPLETE);
        exit(EXIT_FAILURE);

    }

    

    while (!glfwWindowShouldClose(d.w)){
        glfwPollEvents();

        //Draw Frame
        vkWaitForFences(d.logical.device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(d.logical.device, 1, &inFlightFence); 

        uint32_t imageIndex;
        vkAcquireNextImageKHR(d.logical.device, s.swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

        vkResetCommandBuffer(c.buffer, 0);
        RecordCommands(&c.buffer, &p, &s, imageIndex);

        VkSubmitInfo submitInfo = {0};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &c.buffer;

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(d.logical.graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
            vkDestroyCommandPool(d.logical.device, c.pool, NULL);
            DestroyPipeline(&d, &s, &p, COMPLETE);
            DestroySwapChain(&d, &s, COMPLETE);
            ExitVulkan(&d, COMPLETE);
            exit(EXIT_FAILURE);
        }

        VkPresentInfoKHR presentInfo = {0};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &s.swapchain;
        presentInfo.pImageIndices = &imageIndex;

        presentInfo.pResults = NULL;

        vkQueuePresentKHR(d.logical.graphicsQueue, &presentInfo);



    }

    vkQueueWaitIdle(d.logical.graphicsQueue);
    vkDeviceWaitIdle(d.logical.device);
    vkDestroySemaphore(d.logical.device, imageAvailableSemaphore, NULL);
    vkDestroySemaphore(d.logical.device, renderFinishedSemaphore, NULL);
    vkDestroyFence(d.logical.device, inFlightFence, NULL);

    vkDestroyCommandPool(d.logical.device, c.pool, NULL);
    DestroyPipeline(&d, &s, &p, COMPLETE);
    DestroySwapChain(&d, &s, COMPLETE);
    ExitVulkan(&d, COMPLETE);
    return EXIT_SUCCESS;
}
