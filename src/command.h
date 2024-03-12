#ifndef SR_COMMAND_H
#define SR_COMMAND_H

#include <vulkan/vulkan_core.h>
#include "error.h"
#include "init.h"
#include "pipeline.h"
#include "vertex.h"
#include "swap.h"


typedef struct VulkanCommand {
    VkCommandPool pool;
    VkCommandBuffer buffer[SR_MAX_FRAMES_IN_FLIGHT];
    VkSemaphore imageAvalible[SR_MAX_FRAMES_IN_FLIGHT];
    VkSemaphore renderFinished[SR_MAX_FRAMES_IN_FLIGHT];
    VkFence inFlight[SR_MAX_FRAMES_IN_FLIGHT];
} VulkanCommand;

ErrorCode CreateCommand(VulkanCommand* cmd, VulkanContext* c, VulkanDevice* d);
ErrorCode RecordCommandBuffer(SwapChain* s, VulkanPipeline* p, VkCommandBuffer* buffer, GeometryBuffer* verts, uint32_t imageIndex);
void DestroyCommand(VulkanCommand* cmd, VulkanDevice* d);




#endif
