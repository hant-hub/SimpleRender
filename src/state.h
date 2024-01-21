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

typedef struct CommandData {
    uint32_t commandBufferCount;
    VkCommandPool commandPool;
    VkCommandBuffer* commandbuffers;
} CommandData;

typedef struct PipelineData {
    uint32_t renderpassCount;
    VkPipeline pipeline;
    VkRenderPass* renderpasses;
    VkPipelineLayout layout;
    VkViewport view;
    VkRect2D scissor;
}PipelineData;

typedef struct SwapChainData {
    uint32_t imageCount;
    uint32_t swapCount;
    VkSwapchainKHR* swapchains;
    VkImageView* swapViews;
    VkFramebuffer* frameBuffers;
    VkExtent2D extent;
    VkFormat format;
}SwapChainData;

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



typedef struct VulkanState {
    InitStage stage;
    GLFWwindow* w;
    
    VkDebugUtilsMessengerEXT debugMessenger;   

    VkInstance instance;
    VkSurfaceKHR surface;
    PhysicalData physical;
    LogicalData logical;

    PipelineData pipe;
    CommandData command;
    SwapChainData swapData;
    
} VulkanState;



    








#endif
