#include "init.h"
#include "instance.h"
#include <vulkan/vulkan_core.h>


inline static void CloseGLFW(GLFWwindow *w) {
    glfwDestroyWindow(w);
    glfwTerminate();
}

static void CreateSurface(VkInstance* instance, GLFWwindow* window, VkSurfaceKHR* surface) {
    if (glfwCreateWindowSurface(*instance, window, NULL, surface) != VK_SUCCESS) {
        errno = FailedCreation;
        fprintf(stderr, ERR_COLOR("Failed to Create Window Surface!"));
    }
    fprintf(stdout, TRACE_COLOR("Window Surface Created"));

}

void ExitVulkan(VulkanState *state) {
   
    switch (state->stage){
        
        case COMPLETE:
        case COMMAND:
        case FRAMEBUFFER:
            for (int i = 0; i < state->swapData.imageCount; i++) 
                vkDestroyFramebuffer(state->logical.device, state->swapData.frameBuffers[i], NULL);      
            free(state->swapData.frameBuffers);
        case GRAPHPIPE:
        case RENDERPASS:
            for (int i = 0; i < state->pipe.renderpassCount; i++)
                vkDestroyRenderPass(state->logical.device, state->pipe.renderpasses[i], NULL);
            free(state->pipe.renderpasses);
        case IMAGEVIEWS:
            for (int i = 0; i < state->swapData.imageCount; i++) 
                vkDestroyImageView(state->logical.device, state->swapData.swapViews[i], NULL);
            free(state->swapData.swapViews);
            
        case SWAPCHAIN:
            vkDestroySwapchainKHR(state->logical.device, *state->swapData.swapchains, NULL);
            free(state->swapData.swapchains);
        case LOGIDEVICE:
            vkDestroyDevice(state->logical.device, NULL);
        case PHYSDEVICE:
        case SURFACE:
            vkDestroySurfaceKHR(state->instance, state->surface, NULL);

        case DEBUG:
            DestroyDebugMessenger(&state->instance, &state->debugMessenger);

        case INSTANCE:
            vkDestroyInstance(state->instance, NULL);
        case NONE:
            CloseGLFW(state->w);
            glfwTerminate();
        default:
            break;
    }
}

void InitVulkan(VulkanState* state) {

    //appinfo
    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Renderer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    //Create Instance
    CreateInstance(&state->instance, &appInfo);
    if (errno == FailedCreation) {
        ExitVulkan(state);
        return;
    }
    
    //Debug Messenging
    CreateDebugMessenger(&state->debugMessenger, &state->instance);
    if (errno == FailedCreation) {
        ExitVulkan(state);
        return;
    }
    

    //window surface creation
    CreateSurface(&state->instance, state->w, &state->surface);
    if (errno == FailedCreation) {
        ExitVulkan(state);
        return;
    }



    //Physical Device Creation
    pickPhysicalDevice(&state->physical.device, &state->instance, &state->surface);
    if (errno == FailedSearch) {
        ExitVulkan(state);
        return;
    }

    //Logical Device and Queue Creation
    createLogicalDevice(&state->logical.device, &state->logical.graphicsQueue, 
                        &state->logical.presentQueue, &state->physical.device, 
                        &state->surface, &state->physical.indicies);
    if (errno == FailedCreation) {
        ExitVulkan(state);
        return;
    }


    //SwapChain Creation
    state->swapData.swapCount = 1;
    state->swapData.swapchains = (VkSwapchainKHR*)malloc(sizeof(VkSwapchainKHR) * state->swapData.swapCount);
    CreateSwapChain(state->swapData.swapchains, &state->swapData, state->physical.device, state->logical.device, state->surface, state->w);
    if (errno == FailedCreation) {
        ExitVulkan(state);
        return;
    }

    vkGetSwapchainImagesKHR(state->logical.device, *state->swapData.swapchains, &state->swapData.imageCount, NULL); 
    VkImage swapChainImages[state->swapData.imageCount];
    vkGetSwapchainImagesKHR(state->logical.device, *state->swapData.swapchains, &state->swapData.imageCount, swapChainImages);

    //SwapChain Image Views
    state->swapData.swapViews = (VkImageView*)malloc(sizeof(VkImageView) * state->swapData.imageCount);
    CreateImageViews(state->swapData.swapViews, swapChainImages, state->swapData, state->logical.device, state->swapData.imageCount);
    if (errno == FailedCreation) {
        ExitVulkan(state);
        return;
    }

    //Create Render passes
    state->pipe.renderpassCount = 1;
    state->pipe.renderpasses = (VkRenderPass*)malloc(sizeof(VkRenderPass) * state->pipe.renderpassCount);
    CreateRenderPass(state->pipe.renderpasses, state->logical.device, &state->swapData);
    if (errno == FailedCreation) {
        ExitVulkan(state);
        return;
    }


    //create Graphics Pipeline
    CreateGraphicsPipeline(&state->pipe.pipeline, 
            *state->pipe.renderpasses, state->logical.device, 
            state->swapData, &state->pipe, state->w);
    if (errno == FailedCreation) {
        ExitVulkan(state);
        return;
    }


    //Framebuffers
    state->swapData.frameBuffers = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * state->swapData.imageCount);
    CreateFrameBuffers(state->swapData.frameBuffers, state->logical.device, &state->swapData, state->pipe.renderpasses, state->swapData.swapViews, state->swapData.imageCount);
    if (errno == FailedCreation) {
        ExitVulkan(state);
        return;
    }


    //Create Command Storage
    state->command.commandBufferCount = 1;
    state->command.commandbuffers = (VkCommandBuffer*)malloc(sizeof(VkCommandBuffer) * state->command.commandBufferCount);
    CreateCommandObjects(&state->command.commandPool, state->command.commandbuffers, &state->physical.indicies, state->logical.device);
    if (errno == FailedCreation) {
        ExitVulkan(state);
        return;
    }
}


