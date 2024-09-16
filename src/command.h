#ifndef SR_COMMAND_H
#define SR_COMMAND_H

#include <vulkan/vulkan_core.h>
#include "error.h"
#include "init.h"
#include "pipeline.h"
#include "config.h"


typedef struct VulkanCommand {
    VkCommandPool pool;
    VkCommandBuffer buffer[SR_MAX_FRAMES_IN_FLIGHT];
    VkSemaphore imageAvalible[SR_MAX_FRAMES_IN_FLIGHT];
    VkSemaphore renderFinished[SR_MAX_FRAMES_IN_FLIGHT];
    VkFence inFlight[SR_MAX_FRAMES_IN_FLIGHT];
} VulkanCommand;



VkCommandBuffer beginSingleTimeCommand(VkDevice d, VkCommandPool pool);
void endSingleTimeCommand(VkCommandBuffer cmd, VkCommandPool pool, VulkanDevice* d);

ErrorCode CreateCommand(VulkanCommand* cmd, VulkanContext* c, VulkanDevice* d);
void DestroyCommand(VulkanCommand* cmd, VulkanDevice* d);




#endif
