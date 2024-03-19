#include "../command.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include "pipeline.h"
#include "swap.h"
#include "vertex.h"
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
    allocInfo.commandBufferCount = SR_MAX_FRAMES_IN_FLIGHT;

    if (vkAllocateCommandBuffers(d->l, &allocInfo, cmd->buffer) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Allocate Command Buffer");
        return SR_CREATE_FAIL;
    }
    SR_LOG_DEB("Created Command Buffers");


    VkFenceCreateInfo fenceInfo = {0};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkSemaphoreCreateInfo semaphoreInfo = {0};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (unsigned int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateFence(d->l, &fenceInfo, NULL, &cmd->inFlight[i]) != VK_SUCCESS ||
                vkCreateSemaphore(d->l, &semaphoreInfo, NULL, &cmd->imageAvalible[i]) != VK_SUCCESS ||
                vkCreateSemaphore(d->l, &semaphoreInfo, NULL, &cmd->renderFinished[i]) != VK_SUCCESS) {
            SR_LOG_ERR("Failed to Create all Sync Objects");
            return SR_CREATE_FAIL;
        }
    }
    SR_LOG_DEB("Sync Objects Created");

    return SR_NO_ERROR;
}


ErrorCode RecordCommandBuffer(SwapChain* s, VulkanPipeline* p, VulkanPipelineConfig* config, VkCommandBuffer* buffer, GeometryBuffer* verts, uint32_t imageIndex, uint32_t frame) {
    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    if (vkBeginCommandBuffer(*buffer, &beginInfo) != VK_SUCCESS) {
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

    vkCmdBeginRenderPass(*buffer, &renderInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(*buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, p->pipeline);

    VkBuffer vertBufs[] = {verts->vertexBuffer.buf};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(*buffer, 0, 1, vertBufs, offsets);
    vkCmdBindIndexBuffer(*buffer, verts->indexBuffer.buf, 0, VK_INDEX_TYPE_UINT16); 

    VkViewport viewport = {0};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = s->extent.width;
    viewport.height = s->extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(*buffer, 0, 1, &viewport);

    VkRect2D scissor = {0};
    scissor.offset = (VkOffset2D){0, 0};
    scissor.extent = s->extent;
    vkCmdSetScissor(*buffer, 0, 1, &scissor);

    vkCmdBindDescriptorSets(*buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, config->layout, 0, 1, &config->descriptorSet[frame], 0, NULL);

    vkCmdDrawIndexed(*buffer, verts->indexCount, 1, 0, 0, 0);
    vkCmdEndRenderPass(*buffer);


    if (vkEndCommandBuffer(*buffer) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to End Command Buffer");
        return SR_CREATE_FAIL;
    }
    //SR_LOG_DEB("Command Buffer recorded");

    return SR_NO_ERROR;
}


void DestroyCommand(VulkanCommand* cmd, VulkanDevice* d){
    for (unsigned int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyFence(d->l, cmd->inFlight[i], NULL);
        vkDestroySemaphore(d->l, cmd->renderFinished[i], NULL);
        vkDestroySemaphore(d->l, cmd->imageAvalible[i], NULL);
    }
    SR_LOG_DEB("Destroyed Sync Objects");


    vkDestroyCommandPool(d->l, cmd->pool, NULL);
    SR_LOG_DEB("Destroyed Command Pool");
}



VkCommandBuffer beginSingleTimeCommand(VkDevice d, VkCommandPool pool) {
    VkCommandBufferAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = pool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer cmd;
    vkAllocateCommandBuffers(d, &allocInfo, &cmd);

    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd, &beginInfo);

    return cmd;
}

void endSingleTimeCommand(VkCommandBuffer cmd, VkCommandPool pool, VulkanDevice* d) {
    vkEndCommandBuffer(cmd);

    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    vkQueueSubmit(d->graph, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(d->graph);

    vkFreeCommandBuffers(d->l, pool, 1, &cmd);
}
