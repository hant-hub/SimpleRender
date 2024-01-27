#ifndef SR_INIT_H
#define SR_INIT_H
#include "error.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

typedef struct {
    VkInstance instance;
#ifdef DEBUG
    VkDebugUtilsMessengerEXT debug;
#endif
} VulkanContext;

#ifdef DEBUG
void DestroyDebugMessenger(VulkanContext* context); 
#endif



ErrorCode CreateInstance(VulkanContext* context);


#endif
