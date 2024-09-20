#include "../command.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include "pipeline.h"
#include <vulkan/vulkan_core.h>




ErrorCode CreateCommand(VulkanCommand* cmd){
    VulkanDevice* d = &sr_device;
    VulkanContext*c = &sr_context;
    
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


void DestroyCommand(VulkanCommand* cmd){
    VkDevice d = sr_device.l;
    for (unsigned int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyFence(d, cmd->inFlight[i], NULL);
        vkDestroySemaphore(d, cmd->renderFinished[i], NULL);
        vkDestroySemaphore(d, cmd->imageAvalible[i], NULL);
    }
    SR_LOG_DEB("Destroyed Sync Objects");


    vkDestroyCommandPool(d, cmd->pool, NULL);
    SR_LOG_DEB("Destroyed Command Pool");
}



VkCommandBuffer beginSingleTimeCommand(VkCommandPool pool) {
    VkCommandBufferAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = pool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer cmd;
    vkAllocateCommandBuffers(sr_device.l, &allocInfo, &cmd);

    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd, &beginInfo);

    return cmd;
}

void endSingleTimeCommand(VkCommandBuffer cmd, VkCommandPool pool) {
    vkEndCommandBuffer(cmd);

    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    vkQueueSubmit(sr_device.graph, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(sr_device.graph);

    vkFreeCommandBuffers(sr_device.l, pool, 1, &cmd);
}
