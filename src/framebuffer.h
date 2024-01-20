#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include "Vulkan.h"
#include "error.h"
#include "swapchain.h"
#include <vulkan/vulkan_core.h>

static void CreateFrameBuffers(VkFramebuffer* framebuffers, VkDevice logicalDevice, SwapImageDetails* details, 
                                VkRenderPass* renderPass, VkImageView* swapViews, uint32_t imageCount) {
    for (int i = 0; i < imageCount; i++) {
        VkImageView attachments[] = {
            swapViews[i]
        };

        VkFramebufferCreateInfo frameBufferInfo = {};
        frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferInfo.renderPass = *renderPass;
        frameBufferInfo.attachmentCount = 1;
        frameBufferInfo.pAttachments = attachments;
        frameBufferInfo.width = details->extent.width;
        frameBufferInfo.height = details->extent.height;
        frameBufferInfo.layers = 1;

        if (vkCreateFramebuffer(logicalDevice, &frameBufferInfo, NULL, &framebuffers[i]) != VK_SUCCESS) {
            for (int j = i-1; j >= 0; j--) {
                vkDestroyFramebuffer(logicalDevice, framebuffers[j], NULL);
            }
            errno = FailedCreation;
            fprintf(stderr, ERR_COLOR("Failed to Initialize all FrameBuffers"));
            return;
        }

    }
    fprintf(stdout, TRACE_COLOR("Framebuffers Created"));
    return;
}

#endif
