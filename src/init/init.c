#include "../init.h"
#include "error.h"
#include "instance.h"
#include "../state.h"
#include <vulkan/vulkan_core.h>


inline static void CloseGLFW(GLFWwindow *w) {
    glfwDestroyWindow(w);
    glfwTerminate();
}

static ErrorCode CreateSurface(VkInstance* instance, GLFWwindow* window, VkSurfaceKHR* surface) {
    if (glfwCreateWindowSurface(*instance, window, NULL, surface) != VK_SUCCESS) {
        fprintf(stderr, ERR_COLOR("Failed to Create Window Surface!"));
        return Error;
    }
    fprintf(stdout, TRACE_COLOR("Window Surface Created"));
    return NoError;

}

void ExitVulkan(VulkanDevice* deviceData, InitStage stage) {
    switch (stage) {
        default:
        case LOGIDEVICE:
            vkDestroyDevice(deviceData->logical.device, NULL);
        case PHYSDEVICE:
        case SURFACE:
            vkDestroySurfaceKHR(deviceData->instance, deviceData->surface, NULL);
        case DEBUG:
            DestroyDebugMessenger(&deviceData->instance, &deviceData->debugMessenger);
        case INSTANCE:
            vkDestroyInstance(deviceData->instance, NULL);
        case NONE:
            CloseGLFW(deviceData->w);
            glfwTerminate();
    }
}

ErrorCode InitVulkan(VulkanDevice* deviceData) {
    //appinfo
    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Renderer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    //Create Instance
    ErrorCode result = CreateInstance(&deviceData->instance, &appInfo);
    if (result == Error) {
        ExitVulkan(deviceData, INSTANCE);
        return Error;
    }
    
    //Debug Messenging
    result = CreateDebugMessenger(&deviceData->debugMessenger, &deviceData->instance); 
    if (result == Error) {
        ExitVulkan(deviceData, DEBUG);
        return Error;
    }
    

    //window surface creation
    result = CreateSurface(&deviceData->instance, deviceData->w, &deviceData->surface);
    if (result == Error) {
        ExitVulkan(deviceData, SURFACE);
        return Error;
    }



    //Physical Device Creation
    result = pickPhysicalDevice(&deviceData->physical.device, &deviceData->instance, &deviceData->surface);
    if (result == Error) {
        ExitVulkan(deviceData, PHYSDEVICE);
        return Error;
    }

    //Logical Device and Queue Creation
    result = createLogicalDevice(&deviceData->logical.device, &deviceData->logical.graphicsQueue, 
                        &deviceData->logical.presentQueue, &deviceData->physical.device, 
                        &deviceData->surface, &deviceData->physical.indicies);
    if (result == Error) {
        ExitVulkan(deviceData, LOGIDEVICE);
        return Error;
    }

    return NoError;
}

void DestroySwapChain(VulkanDevice* d, SwapChain* s, InitStage stage) {
    switch (stage) {
        default:
        case IMAGEVIEWS:
            for (int i = 0; i < s->imageCount; i++)
                vkDestroyImageView(d->logical.device, s->swapViews[i], NULL);
        case SWAPCHAIN:
            vkDestroySwapchainKHR(d->logical.device, s->swapchain, NULL);
            return;
    }
}

ErrorCode CreateSwapChain(VulkanDevice* d, SwapChain* s, InitStage stage){
    switch (stage) {
        default:
        case SWAPCHAIN: {

            //SwapChain Creation
            uint32_t formatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(d->physical.device, d->surface, &formatCount, NULL);
            VkSurfaceFormatKHR formats[formatCount];
            if (formatCount != 0) {
                vkGetPhysicalDeviceSurfaceFormatsKHR(d->physical.device, d->surface, &formatCount, formats);
            }

            VkSurfaceFormatKHR format = formats[0];
            for (int i = 0; i < formatCount; i++) {
                if (formats[i].format == VK_FORMAT_B8G8R8_SRGB &&
                        formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    format = formats[i];
                }
            }

            uint32_t presentCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(d->physical.device, d->surface, &presentCount, NULL);
            VkPresentModeKHR presentmodes[presentCount];
            if (formatCount != 0) {
                vkGetPhysicalDeviceSurfacePresentModesKHR(d->physical.device, d->surface, &presentCount, presentmodes);
            }

            VkPresentModeKHR mode = VK_PRESENT_MODE_FIFO_KHR;    
            for (int i = 0; i < presentCount; i++) {
                if (presentmodes[i] == VK_PRESENT_MODE_MAILBOX_KHR) 
                    mode = presentmodes[i];
            }

            VkSurfaceCapabilitiesKHR capabilities;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(d->physical.device, d->surface, &capabilities);
            VkExtent2D extent = {0, 0};

            if (capabilities.currentExtent.width != UINT32_MAX) {
                extent = capabilities.currentExtent;
            } else {
                int width, height;
                glfwGetFramebufferSize(d->w, &width, &height);

                extent.width = (uint32_t)width;
                extent.height = (uint32_t)height;

                extent.width = clampf(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
                extent.height = clampf(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
            }

            uint32_t imageCount = capabilities.minImageCount + 1;
            if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
                imageCount = capabilities.maxImageCount;
            }

            VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
            swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapchainCreateInfo.surface = d->surface;

            swapchainCreateInfo.minImageCount = imageCount;
            swapchainCreateInfo.imageFormat = format.format;
            swapchainCreateInfo.imageColorSpace = format.colorSpace;
            swapchainCreateInfo.imageExtent = extent;
            swapchainCreateInfo.imageArrayLayers = 1;
            swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            s->extent = extent;
            s->format = format.format;

            QueueFamilyIndicies indicies = findQueueFamily(&d->physical.device, &d->surface); 
            uint32_t values[] = {indicies.graphicsQueue.value, indicies.presentQueue.value};
            if (values[0] != values[1]) {
                swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                swapchainCreateInfo.queueFamilyIndexCount = 2;
                swapchainCreateInfo.pQueueFamilyIndices = values;
            } else {
                swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                swapchainCreateInfo.queueFamilyIndexCount = 0;
                swapchainCreateInfo.pQueueFamilyIndices = NULL;
            }

            swapchainCreateInfo.preTransform = capabilities.currentTransform;
            swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            swapchainCreateInfo.presentMode = mode;
            swapchainCreateInfo.clipped = VK_TRUE;
            swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

            if (vkCreateSwapchainKHR(d->logical.device, &swapchainCreateInfo, NULL, &s->swapchain) != VK_SUCCESS) {
                fprintf(stderr, ERR_COLOR("Swap Chain Creation Failed"));
                DestroySwapChain(d, s, SWAPCHAIN);
                return Error;
            }
            fprintf(stdout, TRACE_COLOR("Swap Chain Created"));
                        }

        case IMAGEVIEWS: {

            //SwapChain Image Views
            ErrorCode result = CreateImageViews(d->logical.device, s);
            if (result == Error) {
                DestroySwapChain(d, s, IMAGEVIEWS);
                return Error;
            }


            return NoError;
                         }
    }
}

void DestroyPipeline(VulkanDevice* d, SwapChain* s, Pipeline* p, InitStage stage) {
    switch (stage) {
        default:
        case RENDERPASS:
            vkDestroyRenderPass(d->logical.device, p->renderpass, NULL);
        case FRAMEBUFFER:
            for (int i = 0; i < s->imageCount; i++)
                vkDestroyFramebuffer(d->logical.device, s->frameBuffers[i], NULL);
        case GRAPHPIPE:
            vkDestroyPipeline(d->logical.device, p->pipeline, NULL);
            vkDestroyPipelineLayout(d->logical.device, p->layout, NULL);
            return;
    }

}

ErrorCode CreatePipeline(VulkanDevice* d, SwapChain* s, Pipeline* p, InitStage stage) {

    ErrorCode result;
    switch (stage) {
        default:
        case RENDERPASS:
            //Create Render passes
            result = CreateRenderPass(&p->renderpass, d->logical.device, s->format);
            if (result == Error) {
                DestroyPipeline(d, s, p, RENDERPASS);
                return Error;
            }
        case FRAMEBUFFER:
            s->frameBuffers = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * s->imageCount);
            memset(s->frameBuffers, 0, sizeof(VkFramebuffer) * s->imageCount);
            //Framebuffers
            result = CreateFrameBuffers(d->logical.device, s, &p->renderpass);
            if (result == Error) {
                DestroySwapChain(d, s, FRAMEBUFFER);
                return Error;
            }

        case GRAPHPIPE:
            //create Graphics Pipeline
            result = CreateGraphicsPipeline(d, s, p);
            if (result == Error) {
                DestroyPipeline(d, s, p, GRAPHPIPE);
                return Error;
            }

            return NoError;
    }

}


ErrorCode CreateCommandObjects(Command* c, uint32_t bufferCount, VulkanDevice* d) {
    c->buffers = (VkCommandBuffer*)malloc(sizeof(VkCommandBuffer) * bufferCount);
    //initialize Command pool
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = d->physical.indicies.graphicsQueue.value;

    if (vkCreateCommandPool(d->logical.device, &poolInfo, NULL, &c->pool) != VK_SUCCESS) {
        fprintf(stderr, ERR_COLOR("Failed to create Command Pool"));
        return Error;
    }
    fprintf(stdout, TRACE_COLOR("Created Command Pool"));

    //Allocate Command Buffer
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = c->pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = bufferCount;

    if (vkAllocateCommandBuffers(d->logical.device, &allocInfo, c->buffers) != VK_SUCCESS) {
        fprintf(stderr, ERR_COLOR("Failed to create Command Buffer"));
        free(c->buffers);
        return Error;
    }
    fprintf(stdout, TRACE_COLOR("Created Command Buffer"));
    return NoError;
}

ErrorCode RecordCommands(VkCommandBuffer* commandBuffer, Pipeline* p, SwapChain* s, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    if (vkBeginCommandBuffer(*commandBuffer, &beginInfo) != VK_SUCCESS) {
        fprintf(stderr, ERR_COLOR("Failed to Begin Recording Command Buffer"));
        return Error;
    }

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = p->renderpass;
    renderPassInfo.framebuffer = s->frameBuffers[imageIndex];

    renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
    renderPassInfo.renderArea.extent = s->extent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}}; 
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues =  &clearColor;

    vkCmdBeginRenderPass(*commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, p->pipeline);

    vkCmdDraw(*commandBuffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(*commandBuffer);

    if (vkEndCommandBuffer(*commandBuffer) != VK_SUCCESS) {
        fprintf(stderr, ERR_COLOR("Failed to End Recording Command Buffer"));
        return Error;
    }

    return NoError;
}

void DestroySyncObjects(VulkanDevice* d, SyncObjects* s) {
    
    for (int i = 0; i < s->FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(d->logical.device, s->imageAvalible[i], NULL);
        vkDestroySemaphore(d->logical.device, s->renderFinished[i], NULL);
        vkDestroyFence(d->logical.device, s->fences[i], NULL);
    }

    free(s->fences);
    free(s->renderFinished);
    free(s->imageAvalible);
}

ErrorCode CreateSyncObjects(VulkanDevice* d, Command* c, SyncObjects* s, uint32_t MAX_FRAMES_IN_FLIGHT) {
    //Sync Objects
    s->imageAvalible = (VkSemaphore*)malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
    s->renderFinished = (VkSemaphore*)malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
    s->fences = (VkFence*)malloc(sizeof(VkFence) * MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo = {0};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {0};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(d->logical.device, &semaphoreInfo, NULL, &s->imageAvalible[i]) != VK_SUCCESS ||
                vkCreateSemaphore(d->logical.device, &semaphoreInfo, NULL, &s->renderFinished[i]) != VK_SUCCESS ||
                vkCreateFence(d->logical.device, &fenceInfo, NULL, &s->fences[i]) != VK_SUCCESS) {

            fprintf(stderr, ERR_COLOR("Failed to Create Sync objects"));
            for (int j = i; j >= 0; j--) {
                vkDestroySemaphore(d->logical.device, s->imageAvalible[j], NULL);
                vkDestroySemaphore(d->logical.device, s->renderFinished[j], NULL);
                vkDestroyFence(d->logical.device, s->fences[j], NULL);
            }
            free(s->fences);
            free(s->renderFinished);
            free(s->imageAvalible);
            return Error;
        }
    }
    fprintf(stdout, TRACE_COLOR("Sync objects created"));
    s->FRAMES_IN_FLIGHT = MAX_FRAMES_IN_FLIGHT;

    return NoError;
}




