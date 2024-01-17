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
#include <GLFW/glfw3.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>



static const uint32_t WIDTH = 1920;
static const uint32_t HEIGHT = 1080;





static void CreateSurface(VkInstance* instance, GLFWwindow* window, VkSurfaceKHR* surface) {
    if (glfwCreateWindowSurface(*instance, window, NULL, surface) != VK_SUCCESS) {
        errno = FailedCreation;
        fprintf(stderr, ERR_COLOR("Failed to Create Window Surface!"));
    }
    fprintf(stdout, TRACE_COLOR("Window Surface Created"));

}

inline static void CloseGLFW(GLFWwindow *w) {
    glfwDestroyWindow(w);
    glfwTerminate();
}

int main() {

    glfwInit();
    //create Window
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Renderer", glfwGetPrimaryMonitor(), NULL);

    //appinfo
    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Renderer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    //Create Instance
    VkInstance instance;
    CreateInstance(&instance, &appInfo);
    if (errno == FailedCreation) {
        CloseGLFW(window);
        exit(EXIT_FAILURE);
    }
    
    //Debug Messenging
    VkDebugUtilsMessengerEXT debugMessenger;
    CreateDebugMessenger(&debugMessenger, &instance);
    if (errno == FailedCreation) {
        CloseGLFW(window);
        vkDestroyInstance(instance, NULL);
        exit(EXIT_FAILURE);
    }
    

    //window surface creation
    VkSurfaceKHR surface;
    CreateSurface(&instance, window, &surface);
    if (errno == FailedCreation) {
        CloseGLFW(window); 
        if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);
        vkDestroyInstance(instance, NULL);
        exit(EXIT_FAILURE);
    }



    //Physical Device Creation
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    pickPhysicalDevice(&physicalDevice, &instance, &surface);
    if (errno == FailedSearch) {
        CloseGLFW(window);
        if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        exit(EXIT_FAILURE);
    }

    //Logical Device and Queue Creation
    VkDevice logicalDevice = VK_NULL_HANDLE; 
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
    createLogicalDevice(&logicalDevice, &graphicsQueue, &presentQueue, &physicalDevice, &surface);
    if (errno == FailedCreation) {
        CloseGLFW(window);
        if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        exit(EXIT_FAILURE);
    }


    //SwapChain Creation
    VkSwapchainKHR swapchain;
    SwapImageDetails details;
    CreateSwapChain(&swapchain, &details, physicalDevice, logicalDevice, surface, window);
    if (errno == FailedCreation) {
        CloseGLFW(window);
        if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyDevice(logicalDevice, NULL);
        vkDestroyInstance(instance, NULL);
        exit(EXIT_FAILURE);
    }

    uint32_t imageCount;
    vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, NULL); 
    VkImage swapChainImages[imageCount];
    vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, swapChainImages);

    //SwapChain Image Views
    VkImageView swapViews[imageCount];
    CreateImageViews(swapViews, swapChainImages, details, logicalDevice, imageCount);
    if (errno == FailedCreation) {
        CloseGLFW(window);
        if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);
        vkDestroySwapchainKHR(logicalDevice, swapchain, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyDevice(logicalDevice, NULL);
        vkDestroyInstance(instance, NULL);
        exit(EXIT_FAILURE);
    }

    //Create Render passes
    VkRenderPass renderPass;
    CreateRenderPass(&renderPass, logicalDevice, &details);
    if (errno == FailedCreation) {
        CloseGLFW(window);
        if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);
        DestroyImageViews(logicalDevice, swapViews, imageCount);
        vkDestroySwapchainKHR(logicalDevice, swapchain, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyDevice(logicalDevice, NULL);
        vkDestroyInstance(instance, NULL);
        exit(EXIT_FAILURE);
    }






    //create Graphics Pipeline
    PipelineData data;
    VkPipeline pipeline;
    CreateGraphicsPipeline(&pipeline, renderPass, logicalDevice, details, &data, window);
    if (errno == FailedCreation) {
        CloseGLFW(window);
        if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);
        vkDestroyRenderPass(logicalDevice, renderPass, NULL);
        DestroyImageViews(logicalDevice, swapViews, imageCount);
        vkDestroySwapchainKHR(logicalDevice, swapchain, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyDevice(logicalDevice, NULL);
        vkDestroyInstance(instance, NULL);
        exit(EXIT_FAILURE);
    }


    //Framebuffers
    VkFramebuffer framebuffers[imageCount];
    for (int i = 0; i < imageCount; i++) {
        VkImageView attachments[] = {
            swapViews[i]
        };

        VkFramebufferCreateInfo frameBufferInfo = {0};
        frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferInfo.renderPass = renderPass;
        frameBufferInfo.attachmentCount = 1;
        frameBufferInfo.pAttachments = attachments;
        frameBufferInfo.width = details.extent.width;
        frameBufferInfo.height = details.extent.height;
        frameBufferInfo.layers = 1;

        if (vkCreateFramebuffer(logicalDevice, &frameBufferInfo, NULL, &framebuffers[i]) != VK_SUCCESS) {
            CloseGLFW(window);
            if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);
            for (int j = i-1; j >= 0; j--) {
                vkDestroyFramebuffer(logicalDevice, framebuffers[j], NULL);
            }
            vkDestroyRenderPass(logicalDevice, renderPass, NULL);
            DestroyImageViews(logicalDevice, swapViews, imageCount);
            vkDestroySwapchainKHR(logicalDevice, swapchain, NULL);
            vkDestroySurfaceKHR(instance, surface, NULL);
            vkDestroyDevice(logicalDevice, NULL);
            vkDestroyInstance(instance, NULL);
            exit(EXIT_FAILURE);
        }
            
    }
    fprintf(stdout, TRACE_COLOR("Framebuffers Created"));


    

    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();


        glfwSwapBuffers(window);
    }

    if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);

    for (int i = 0; i < imageCount; i++) {
        vkDestroyFramebuffer(logicalDevice, framebuffers[i], NULL);
    }
    

    vkDestroyPipeline(logicalDevice, pipeline, NULL);
    vkDestroyPipelineLayout(logicalDevice, data.layout, NULL);
    vkDestroyRenderPass(logicalDevice, renderPass, NULL);
    DestroyImageViews(logicalDevice, swapViews, imageCount);
    vkDestroySwapchainKHR(logicalDevice, swapchain, NULL);
    vkDestroySurfaceKHR(instance, surface, NULL);
    vkDestroyDevice(logicalDevice, NULL);
    vkDestroyInstance(instance, NULL);
    CloseGLFW(window);

    return EXIT_SUCCESS;
}
