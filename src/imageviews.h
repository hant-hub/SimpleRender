#ifndef IMAGE_VIEWS_H
#define IMAGE_VIEWS_H
#include "Vulkan.h"
#include "state.h"
#include "swapchain.h"

static void CreateImageViews(VkImageView* swapViews, VkImage* swapChainImages, SwapChainData details, VkDevice logicalDevice, uint32_t imageCount) {
    for (int i = 0; i < imageCount; i++) {
        VkImageViewCreateInfo viewCreateInfo = {};
        viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.image = swapChainImages[i];
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format = details.format;

        viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(logicalDevice, &viewCreateInfo, NULL, &swapViews[i]) != VK_SUCCESS) {
            errno = FailedCreation;
            fprintf(stderr, ERR_COLOR("Failed to Create Image View: index=%d"), i);

            for (int j = i; i >= 0; i--) vkDestroyImageView(logicalDevice, swapViews[j], NULL);

            return;
        }
    }
    fprintf(stdout, TRACE_COLOR("Image Views Created"));
}

static void DestroyImageViews(VkDevice logicalDevice, VkImageView* swapViews, uint32_t imageCount) {
    for (int i = 0; i < imageCount; i++) {
        vkDestroyImageView(logicalDevice, swapViews[i], NULL);
    }
}


#endif
