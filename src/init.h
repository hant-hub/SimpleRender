#ifndef SR_INIT_H
#define SR_INIT_H
#include "log.h"
#include "error.h"
#include "util.h"
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


typedef struct {
    VkPhysicalDevice p;
    VkDevice l;
    VkQueue graph;
} VulkanDevice;


static const char* validationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};

void DestroyContext(VulkanContext* context); 
ErrorCode CreateInstance(VulkanContext* context);

void DestroyDevice(VulkanDevice* d);
ErrorCode CreateDevices(VulkanDevice* d, VulkanContext* context);


#endif
