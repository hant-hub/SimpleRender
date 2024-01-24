#ifndef IMAGE_VIEWS_H
#define IMAGE_VIEWS_H
#include "../Vulkan.h"
#include "error.h"
#include "../state.h"
#include <vulkan/vulkan_core.h>

static ErrorCode CreateImageViews(VkDevice logicalDevice, SwapChain* s) {


    vkGetSwapchainImagesKHR(logicalDevice, s->swapchain, &s->imageCount, NULL); 
    s->swapViews = (VkImageView*)malloc(sizeof(VkImageView) * s->imageCount);

    VkImage swapChainImages[s->imageCount];
    vkGetSwapchainImagesKHR(logicalDevice, s->swapchain, &s->imageCount, swapChainImages); 

    for (int i = 0; i < s->imageCount; i++) {
        VkImageViewCreateInfo viewCreateInfo = {};
        viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.image = swapChainImages[i];
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format = s->format;

        viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(logicalDevice, &viewCreateInfo, NULL, &s->swapViews[i]) != VK_SUCCESS) {
            fprintf(stderr, ERR_COLOR("Failed to Create Image View: index=%d"), i);
            for (int j = i; i >= 0; i--) vkDestroyImageView(logicalDevice, s->swapViews[j], NULL);

            return Error;
        }
    }
    fprintf(stdout, TRACE_COLOR("Image Views Created"));
    return NoError;
}

static void DestroyImageViews(VkDevice logicalDevice, VkImageView* swapViews, uint32_t imageCount) {
    for (int i = 0; i < imageCount; i++) {
        vkDestroyImageView(logicalDevice, swapViews[i], NULL);
    }
}


#endif
