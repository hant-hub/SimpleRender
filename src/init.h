#ifndef SR_INIT_H
#define SR_INIT_H
#include "log.h"
#include "error.h"
#include "util.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdlib.h>

typedef struct {
    VkInstance instance;
    VkSurfaceKHR surface;
#ifdef DEBUG
    VkDebugUtilsMessengerEXT debug;
#endif
} VulkanContext;

typedef struct {
    VkSurfaceCapabilitiesKHR capabilities;
    uint32_t formatCount;
    uint32_t modeCount;
    VkSurfaceFormatKHR* formats;
    VkPresentModeKHR* modes;
}SwapChainDetails;

typedef struct {
    VkPhysicalDevice p;
    VkDevice l;
    VkQueue graph;
    VkQueue present;
    SwapChainDetails swapDetails;
} VulkanDevice;


static const char* validationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};

static const char* deviceExtensions[] = {
    "VK_KHR_swapchain",
};


ErrorCode CreateSurface(VulkanContext* context, GLFWwindow* window); 

void DestroyContext(VulkanContext* context); 
ErrorCode CreateInstance(VulkanContext* context);

void DestroyDevice(VulkanDevice* d);
ErrorCode CreateDevices(VulkanDevice* d, VulkanContext* context);


#endif
