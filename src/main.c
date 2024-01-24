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



const int MAX_FRAMES_IN_FLIGHT = 2;



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
    result = CreateSwapChain(&d, &s, COMPLETE); 
    if (result == Error) {
        ExitVulkan(&d, COMPLETE);
        exit(EXIT_FAILURE);
    }

    Pipeline p = {0};
    result = CreatePipeline(&d, &s, &p, COMPLETE);
    if (result == Error) {
        DestroySwapChain(&d, &s, COMPLETE);
        ExitVulkan(&d, COMPLETE);
        exit(EXIT_FAILURE);
    }
    
    Command c = {0};
    result = CreateCommandObjects(&c, MAX_FRAMES_IN_FLIGHT, &d);
    if (result == Error) {
        DestroyPipeline(&d, &s, &p, COMPLETE);
        DestroySwapChain(&d, &s, COMPLETE);
        ExitVulkan(&d, COMPLETE);
        exit(EXIT_FAILURE);
    }
    


    //Sync Objects
    VkSemaphore imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
    VkFence inFlightFences[MAX_FRAMES_IN_FLIGHT];

    VkSemaphoreCreateInfo semaphoreInfo = {0};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {0};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(d.logical.device, &semaphoreInfo, NULL, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(d.logical.device, &semaphoreInfo, NULL, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(d.logical.device, &fenceInfo, NULL, &inFlightFences[i]) != VK_SUCCESS) {
            vkDestroyCommandPool(d.logical.device, c.pool, NULL);
            DestroyPipeline(&d, &s, &p, COMPLETE);
            DestroySwapChain(&d, &s, COMPLETE);
            ExitVulkan(&d, COMPLETE);
            exit(EXIT_FAILURE);

        }
    }

    uint32_t currentFrame = 0; 

    while (!glfwWindowShouldClose(d.w)){
        glfwPollEvents();

        //Draw Frame
        vkWaitForFences(d.logical.device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(d.logical.device, s.swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR){
            vkDeviceWaitIdle(d.logical.device);
            
            DestroyPipeline(&d, &s, &p, FRAMEBUFFER);
            DestroySwapChain(&d, &s, IMAGEVIEWS);

            CreateSwapChain(&d, &s, COMPLETE);
            CreatePipeline(&d, &s, &p, FRAMEBUFFER);
            continue;


        } else if (result != VK_SUCCESS) {
            break;
        }

        vkResetFences(d.logical.device, 1, &inFlightFences[currentFrame]); 





        vkResetCommandBuffer(c.buffers[currentFrame], 0);
        RecordCommands(&c.buffers[currentFrame], &p, &s, imageIndex);

        VkSubmitInfo submitInfo = {0};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &c.buffers[currentFrame];

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(d.logical.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
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
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;


    }

    vkQueueWaitIdle(d.logical.graphicsQueue);
    vkDeviceWaitIdle(d.logical.device);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(d.logical.device, imageAvailableSemaphores[i], NULL);
        vkDestroySemaphore(d.logical.device, renderFinishedSemaphores[i], NULL);
        vkDestroyFence(d.logical.device, inFlightFences[i], NULL);
    }

    vkDestroyCommandPool(d.logical.device, c.pool, NULL);
    free(c.buffers);
    DestroyPipeline(&d, &s, &p, COMPLETE);
    DestroySwapChain(&d, &s, COMPLETE);
    ExitVulkan(&d, COMPLETE);
    return EXIT_SUCCESS;
}
