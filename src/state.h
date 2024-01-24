#ifndef STATE_H
#define STATE_H
#include "Vulkan.h"
#include "physicaldevice.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>


typedef struct PhysicalData {
    VkPhysicalDevice device;
    QueueFamilyIndicies indicies;
} PhysicalData;

typedef struct LogicalData {
    VkDevice device;
    VkViewport view;
    VkRect2D scissor;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
} LogicalData;

typedef enum InitStage {
    NONE = 0,
    INSTANCE,
    DEBUG,
    SURFACE,
    PHYSDEVICE,
    LOGIDEVICE,
    SWAPCHAIN,
    IMAGEVIEWS,
    RENDERPASS,
    GRAPHPIPE,
    FRAMEBUFFER,
    COMMAND,
    COMPLETE
}InitStage;


typedef struct VulkanDevice {
    GLFWwindow* w;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    PhysicalData physical;
    LogicalData logical;
} VulkanDevice;

typedef struct SwapChain {
    VkSwapchainKHR swapchain;
    uint32_t imageCount;
    VkExtent2D extent;
    VkFormat format;
    VkImageView* swapViews;
    VkFramebuffer* frameBuffers;
} SwapChain;

typedef struct Pipeline {
    VkPipeline pipeline;
    VkPipelineLayout layout;
    VkRenderPass renderpass;
} Pipeline;

typedef struct Command {
    VkCommandPool pool;
    uint32_t bufferCount;
    VkCommandBuffer* buffers;
} Command;




#endif
