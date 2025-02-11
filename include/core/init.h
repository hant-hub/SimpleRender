#ifndef SR_INIT_H
#define SR_INIT_H
#include "error.h"
#include "util.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define SR_MAX_FRAMES_IN_FLIGHT 3

typedef struct VulkanCommand {
    VkCommandPool pool;
    VkCommandBuffer buffer[SR_MAX_FRAMES_IN_FLIGHT];
} VulkanCommand;

typedef struct {
    GLFWwindow* w;
    VkInstance instance;
    VkSurfaceKHR surface;
    VulkanCommand cmd;
#ifdef DEBUG
    VkDebugUtilsMessengerEXT debug;
#endif
} VulkanContext;

typedef struct {
    optional(uint32_t) graphicsFamily;
    optional(uint32_t) presentFamily;
} QueueFamilyIndicies;

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
    QueueFamilyIndicies indicies;
} VulkanDevice;


static const char* validationLayers[] = {
    "VK_LAYER_KHRONOS_validation", 
    //"VK_LAYER_LUNARG_api_dump"
};

static const char* deviceExtensions[] = {
    "VK_KHR_swapchain",
};


QueueFamilyIndicies findQueueFamilies(VkPhysicalDevice p, VkSurfaceKHR surface);



ErrorCode CreateVulkan();
void DestroyVulkan();


ErrorCode querySwapDetails(SwapChainDetails* swapDetails, VkPhysicalDevice p, VkSurfaceKHR s);

VkCommandBuffer beginSingleTimeCommand(VkCommandPool pool);
void endSingleTimeCommand(VkCommandBuffer cmd, VkCommandPool pool);

ErrorCode CreateCommand(VulkanCommand* cmd);
void DestroyCommand(VulkanCommand* cmd);

extern uint32_t WIDTH;
extern uint32_t HEIGHT;
extern bool frameBufferResized;
extern VulkanDevice sr_device;
extern VulkanContext sr_context;


#endif
