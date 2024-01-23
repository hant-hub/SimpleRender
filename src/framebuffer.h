#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include "Vulkan.h"
#include "error.h"
#include "state.h"
#include <vulkan/vulkan_core.h>

static ErrorCode CreateFrameBuffers(VkDevice logicalDevice, SwapChain* s, VkRenderPass* renderPass) {
    for (int i = 0; i < s->imageCount; i++) {
        VkImageView attachments[] = {
            s->swapViews[i]
        };

        VkFramebufferCreateInfo frameBufferInfo = {};
        frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferInfo.renderPass = *renderPass;
        frameBufferInfo.attachmentCount = 1;
        frameBufferInfo.pAttachments = attachments;
        frameBufferInfo.width = s->extent.width;
        frameBufferInfo.height = s->extent.height;
        frameBufferInfo.layers = 1;

        if (vkCreateFramebuffer(logicalDevice, &frameBufferInfo, NULL, &s->frameBuffers[i]) != VK_SUCCESS) {
            for (int j = i-1; j >= 0; j--) {
                vkDestroyFramebuffer(logicalDevice, s->frameBuffers[j], NULL);
            }
            fprintf(stderr, ERR_COLOR("Failed to Initialize all FrameBuffers"));
            return Error;
        }

    }
    fprintf(stdout, TRACE_COLOR("Framebuffers Created"));
    return NoError;
}

#endif
