#include "../swap.h"
#include "clamp.h"
#include "error.h"
#include "init.h"
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>
#include "SimpleMath/src/include/misc.h"
#include "../init.h"
#include "log.h"


ErrorCode CreateSwapChain(VulkanDevice* d, VulkanContext* c, SwapChain* s, VkSwapchainKHR old) {
    //Pick Format
    s->format = d->swapDetails.formats[0];
    for (int i = 0; i < d->swapDetails.formatCount; i++) {
        if (d->swapDetails.formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && 
            d->swapDetails.formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            s->format = d->swapDetails.formats[i];
            break;
        }
    }

    //Pick Mode
    s->mode = VK_PRESENT_MODE_FIFO_KHR;

    //pick Extent
    VkSurfaceCapabilitiesKHR capabilities = d->swapDetails.capabilities;
    if (capabilities.currentExtent.width != UINT32_MAX) {
        s->extent = capabilities.currentExtent;
    } else {
        SR_LOG_DEB("unlimited");
        int width, height;
        glfwGetFramebufferSize(c->w, &width, &height);

        s->extent.width = clampi(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.height);
        s->extent.height = clampi(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }
    s->imgCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && s->imgCount > capabilities.maxImageCount) {
        s->imgCount = capabilities.maxImageCount;
    }

    //Create Swapchain
    VkSwapchainCreateInfoKHR swapInfo = {0};
    swapInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapInfo.surface = c->surface;

    swapInfo.minImageCount = s->imgCount;
    swapInfo.imageFormat = s->format.format;
    swapInfo.imageColorSpace = s->format.colorSpace;
    swapInfo.imageExtent = s->extent;
    swapInfo.imageArrayLayers = 1;
    swapInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    QueueFamilyIndicies indicies = findQueueFamilies(d->p, c->surface);
    uint32_t queueFamilyIndicies[] = {indicies.graphicsFamily.val, indicies.presentFamily.val};

    if (indicies.graphicsFamily.val != indicies.presentFamily.val) {
        swapInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapInfo.queueFamilyIndexCount = 2;
        swapInfo.pQueueFamilyIndices = queueFamilyIndicies;
    } else {
        swapInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapInfo.queueFamilyIndexCount = 0;
        swapInfo.pQueueFamilyIndices = NULL;
    }

    swapInfo.preTransform = capabilities.currentTransform;
    swapInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapInfo.presentMode = s->mode;
    swapInfo.clipped = VK_TRUE;
    swapInfo.oldSwapchain = old;

    if (vkCreateSwapchainKHR(d->l, &swapInfo, NULL, &s->swapChain) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Create SwapChain");
        return SR_CREATE_FAIL;
    }
    SR_LOG_DEB("SwapChain Created");


    //Retrieve Swapchain Images
    VkImage images[s->imgCount];
    vkGetSwapchainImagesKHR(d->l, s->swapChain, &s->imgCount, images);


    //Create Image Views
    s->views = (VkImageView*)malloc(sizeof(VkImageView) * s->imgCount);
    for (int i = 0; i < s->imgCount; i++) {
        VkImageViewCreateInfo viewInfo = {0};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = images[i];

        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = s->format.format;

        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; 
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(d->l, &viewInfo, NULL, &s->views[i]) != VK_SUCCESS) {
            for (int j = 0; j <= i; j--){
                vkDestroyImageView(d->l, s->views[i], NULL);
            }
            free(s->views);


            SR_LOG_ERR("Failed to make swap Image View");
            return SR_CREATE_FAIL;
        }
    }
    SR_LOG_DEB("\tImage Views Created");



    
    return SR_NO_ERROR;
}

ErrorCode CreateFrameBuffers(VulkanDevice* d, SwapChain*s, VulkanPipelineConfig* c) {
    s->buffers = (VkFramebuffer*)malloc(s->imgCount*sizeof(VkFramebuffer));
    for (size_t i = 0; i < s->imgCount; i++) {
        VkImageView attachments[] = {
            s->views[i]
        };

        VkFramebufferCreateInfo createInfo = {0};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = c->pass;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = attachments;
        createInfo.width = s->extent.width;
        createInfo.height = s->extent.height;
        createInfo.layers = 1;

        if (vkCreateFramebuffer(d->l, &createInfo, NULL, &s->buffers[i]) != VK_SUCCESS) {
            SR_LOG_ERR("Failed to create Framebuffer");
            for (int j = 0; j <= i; j--){
                vkDestroyFramebuffer(d->l, s->buffers[j], NULL);
            }
            free(s->buffers);
            return SR_CREATE_FAIL;
        }
    }
    SR_LOG_DEB("\tFrameBuffers Created");


    return SR_NO_ERROR;
}

void DestroySwapChain(VkDevice l, SwapChain* s) {
    for (int i = 0; i < s->imgCount; i++) {
        vkDestroyImageView(l, s->views[i], NULL);
    }
    

    if (s->buffers != NULL) {
        for (int i = 0; i < s->imgCount; i++) {
            vkDestroyFramebuffer(l, s->buffers[i], NULL);
        }
        free(s->buffers);
        SR_LOG_DEB("\tFrameBuffers Destroyed");
    }
    free(s->views);
    SR_LOG_DEB("\tImage Views Destroyed");


    vkDestroySwapchainKHR(l, s->swapChain, NULL);
    SR_LOG_DEB("SwapChain Destroyed");
}
