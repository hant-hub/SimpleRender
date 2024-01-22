#include "init.h"
#include "instance.h"
#include "state.h"
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
            vkDestroyCommandPool(state->logical.device, state->command.commandPool, NULL);
        case FRAMEBUFFER:
            for (int i = 0; i < state->swapData.imageCount; i++) 
                vkDestroyFramebuffer(state->logical.device, state->swapData.frameBuffers[i], NULL);      
            free(state->swapData.frameBuffers);
        case GRAPHPIPE:
            vkDestroyPipeline(state->logical.device, state->pipe.pipeline, NULL);
            vkDestroyPipelineLayout(state->logical.device, state->pipe.layout, NULL);
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
    state->stage = NONE;

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
    state->stage = INSTANCE;
    
    //Debug Messenging
    CreateDebugMessenger(&state->debugMessenger, &state->instance);
    if (errno == FailedCreation) {
        ExitVulkan(state);
        return;
    }
    state->stage = DEBUG;
    

    //window surface creation
    CreateSurface(&state->instance, state->w, &state->surface);
    if (errno == FailedCreation) {
        ExitVulkan(state);
        return;
    }
    state->stage = SURFACE;



    //Physical Device Creation
    pickPhysicalDevice(&state->physical.device, &state->instance, &state->surface);
    if (errno == FailedSearch) {
        ExitVulkan(state);
        return;
    }
    state->stage = PHYSDEVICE;

    //Logical Device and Queue Creation
    createLogicalDevice(&state->logical.device, &state->logical.graphicsQueue, 
                        &state->logical.presentQueue, &state->physical.device, 
                        &state->surface, &state->physical.indicies);
    if (errno == FailedCreation) {
        ExitVulkan(state);
        return;
    }
    state->stage = LOGIDEVICE;


    //SwapChain Creation
    state->swapData.swapCount = 1;
    state->swapData.swapchains = (VkSwapchainKHR*)malloc(sizeof(VkSwapchainKHR) * state->swapData.swapCount);
    CreateSwapChain(state->swapData.swapchains, &state->swapData, state->physical.device, state->logical.device, state->surface, state->w);
    if (errno == FailedCreation) {
        ExitVulkan(state);
        return;
    }
    state->stage = SWAPCHAIN;



    //SwapChain Image Views
    CreateImageViews(&state->swapData, state->logical.device);
    if (errno == FailedCreation) {
        ExitVulkan(state);
        return;
    }
    state->stage = IMAGEVIEWS;

    //Create Render passes
    state->pipe.renderpassCount = 1;
    state->pipe.renderpasses = (VkRenderPass*)malloc(sizeof(VkRenderPass) * state->pipe.renderpassCount);
    CreateRenderPass(state->pipe.renderpasses, state->logical.device, state->swapData.format);
    if (errno == FailedCreation) {
        ExitVulkan(state);
        return;
    }
    state->stage = RENDERPASS;


    //create Graphics Pipeline
    CreateGraphicsPipeline(&state->pipe.pipeline, 
            *state->pipe.renderpasses, state->logical.device, 
            state->swapData.extent, &state->pipe, state->w);
    if (errno == FailedCreation) {
        ExitVulkan(state);
        return;
    }
    state->stage = GRAPHPIPE;


    //Framebuffers
    state->swapData.frameBuffers = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * state->swapData.imageCount);
    CreateFrameBuffers(state->swapData.frameBuffers, state->logical.device, &state->swapData, state->pipe.renderpasses, state->swapData.swapViews, state->swapData.imageCount);
    if (errno == FailedCreation) {
        ExitVulkan(state);
        return;
    }
    state->stage = FRAMEBUFFER;


    //Create Command Storage
    state->command.commandBufferCount = 1;
    state->command.commandbuffers = (VkCommandBuffer*)malloc(sizeof(VkCommandBuffer) * state->command.commandBufferCount);
    CreateCommandObjects(&state->command.commandPool, state->command.commandbuffers, &state->physical.indicies, state->logical.device);
    if (errno == FailedCreation) {
        ExitVulkan(state);
        return;
    }
    state->stage = COMPLETE;
}



