#ifndef COMMAND_H
#define COMMAND_H
#include "Vulkan.h"
#include "physicaldevice.h"
#include "swapchain.h"



static void CreateCommandObjects(VkCommandPool* commandPool, VkCommandBuffer* commandBuffer, QueueFamilyIndicies* indicies, VkDevice logicalDevice
                                 ) {
    //initialize Command pool
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = indicies->graphicsQueue.value;

    if (vkCreateCommandPool(logicalDevice, &poolInfo, NULL, commandPool) != VK_SUCCESS) {
        errno = FailedCreation;
        fprintf(stderr, ERR_COLOR("Failed to create Command Pool"));
        return;
    }
    fprintf(stdout, TRACE_COLOR("Created Command Pool"));

    //Allocate Command Buffer
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = *commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, commandBuffer) != VK_SUCCESS) {
        errno = FailedCreation;
        fprintf(stderr, ERR_COLOR("Failed to create Command Buffer"));
        return;
    }
    fprintf(stdout, TRACE_COLOR("Created Command Buffer"));
}

static void RecordCommands(VkCommandBuffer* commandBuffer, VkRenderPass renderPass, VkPipeline *pipeline, SwapImageDetails *details, VkFramebuffer* framebuffers,
                           uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    if (vkBeginCommandBuffer(*commandBuffer, &beginInfo) != VK_SUCCESS) {
        errno = FailedCreation;
        fprintf(stderr, ERR_COLOR("Failed to Begin Recording Command Buffer"));
        return;
    }

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffers[imageIndex];

    renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
    renderPassInfo.renderArea.extent = details->extent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}}; 
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues =  &clearColor;

    vkCmdBeginRenderPass(*commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);

    vkCmdDraw(*commandBuffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(*commandBuffer);

    if (vkEndCommandBuffer(*commandBuffer) != VK_SUCCESS) {
        errno = FailedCreation;
        fprintf(stderr, ERR_COLOR("Failed to End Recording Command Buffer"));
        return;
    }

    fprintf(stdout, TRACE_COLOR("Successfully Recorded Command Buffer"));
}

#endif
