#ifndef SR_COMMAND_H
#define SR_COMMAND_H

#include <vulkan/vulkan_core.h>
#include "error.h"
#include "init.h"
#include "pipeline.h"
#include "swap.h"

typedef struct {
    VkCommandPool pool;
    VkCommandBuffer buffer;
    VkSemaphore imageAvalible;
    VkSemaphore renderFinished;
    VkFence inFlight;
} VulkanCommand;

ErrorCode CreateCommand(VulkanCommand* cmd, VulkanContext* c, VulkanDevice* d);
ErrorCode RecordCommandBuffer(SwapChain* s, VulkanPipeline* p, VulkanCommand* cmd, uint32_t imageIndex); 
void DestroyCommand(VulkanCommand* cmd, VulkanDevice* d);



#endif
