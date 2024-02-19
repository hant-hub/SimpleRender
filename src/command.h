#ifndef SR_COMMAND_H
#define SR_COMMAND_H

#include <vulkan/vulkan_core.h>
#include "error.h"
#include "init.h"

typedef struct {
    VkCommandPool pool;
    VkCommandBuffer buffer;
} VulkanCommand;

ErrorCode CreateCommand(VulkanCommand* cmd, VulkanContext* c, VulkanDevice* d);
void DestroyCommand(VulkanCommand* cmd, VulkanDevice* d);



#endif
