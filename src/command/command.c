#include "../command.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include "pipeline.h"
#include "swap.h"
#include <vulkan/vulkan_core.h>




ErrorCode CreateCommand(VulkanCommand* cmd, VulkanContext* c, VulkanDevice* d){
    
    VkCommandPoolCreateInfo poolInfo = {0};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = d->indicies.graphicsFamily.val;

    if (vkCreateCommandPool(d->l, &poolInfo, NULL, &cmd->pool) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Create Command Pool");
        return SR_CREATE_FAIL;
    }
    SR_LOG_DEB("Command Pool Created");


    VkCommandBufferAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = cmd->pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(d->l, &allocInfo, &cmd->buffer) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Allocate Command Buffer");
        return SR_CREATE_FAIL;
    }
    SR_LOG_DEB("Created Command Buffers");


    VkFenceCreateInfo fenceInfo = {0};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkSemaphoreCreateInfo semaphoreInfo = {0};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateFence(d->l, &fenceInfo, NULL, &cmd->inFlight) != VK_SUCCESS ||
        vkCreateSemaphore(d->l, &semaphoreInfo, NULL, &cmd->imageAvalible) != VK_SUCCESS ||
        vkCreateSemaphore(d->l, &semaphoreInfo, NULL, &cmd->renderFinished) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Create all Sync Objects");
        return SR_CREATE_FAIL;
    }
    SR_LOG_DEB("Sync Objects Created");

    return SR_NO_ERROR;
}


ErrorCode RecordCommandBuffer(SwapChain* s, VulkanPipeline* p, VulkanCommand* cmd, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    if (vkBeginCommandBuffer(cmd->buffer, &beginInfo) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Begin Command Buffer");
        return SR_CREATE_FAIL;
    }
    
    VkRenderPassBeginInfo renderInfo = {0};
    renderInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderInfo.renderPass = p->pass;
    renderInfo.framebuffer = s->buffers[imageIndex];
    renderInfo.renderArea.offset = (VkOffset2D){0, 0};
    renderInfo.renderArea.extent = s->extent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderInfo.clearValueCount = 1;
    renderInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(cmd->buffer, &renderInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmd->buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, p->pipeline);

    VkViewport viewport = {0};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = s->extent.width;
    viewport.height = s->extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmd->buffer, 0, 1, &viewport);

    VkRect2D scissor = {0};
    scissor.offset = (VkOffset2D){0, 0};
    scissor.extent = s->extent;
    vkCmdSetScissor(cmd->buffer, 0, 1, &scissor);

    vkCmdDraw(cmd->buffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(cmd->buffer);


    if (vkEndCommandBuffer(cmd->buffer) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to End Command Buffer");
        return SR_CREATE_FAIL;
    }
    //SR_LOG_DEB("Command Buffer recorded");

    return SR_NO_ERROR;
}


void DestroyCommand(VulkanCommand* cmd, VulkanDevice* d){
    vkDestroyFence(d->l, cmd->inFlight, NULL);
    vkDestroySemaphore(d->l, cmd->renderFinished, NULL);
    vkDestroySemaphore(d->l, cmd->imageAvalible, NULL);
    SR_LOG_DEB("Destroyed Sync Objects");

    vkDestroyCommandPool(d->l, cmd->pool, NULL);
    SR_LOG_DEB("Destroyed Command Pool");
}
