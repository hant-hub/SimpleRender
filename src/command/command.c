#include "error.h"
#include "init.h"
#include "log.h"
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
    return SR_NO_ERROR;
}


void DestroyCommand(VulkanCommand* cmd){
    VkDevice d = sr_device.l;
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

