#ifndef SR_PIPE_H
#define SR_PIPE_H

#include "error.h"
#include "init.h"
#include "config.h"
#include "log.h"
#include "error.h"
#include "util.h"
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>



typedef struct {
    VkViewport view;
    VkRect2D scissor;
    VkPipeline pipeline;
    RenderPass* pass;
} VulkanPipeline;

typedef struct {
    VkSurfaceFormatKHR format;
    VkPresentModeKHR mode;
    VkExtent2D extent;
    uint32_t imgCount;
    VkSwapchainKHR swapChain;
    VkImageView* views;
    VkFramebuffer* buffers;
} SwapChain;


ErrorCode CreateSwapChain(VulkanDevice* d, VulkanContext* c, RenderPass* r, SwapChain* s, VkSwapchainKHR old);
//ErrorCode CreateFrameBuffers(VulkanDevice* d, SwapChain*s, RenderPass* r);
ErrorCode CreateShaderProg(VkDevice d, const char* vertex, const char* frag, VulkanShader* s);
ErrorCode CreatePipeline(VulkanDevice* d, VulkanShader* s, VulkanPipelineConfig* con, VulkanPipeline* p, RenderPass* r);

void DestroySwapChain(VkDevice l, SwapChain* s);
void DestroyShaderProg(VkDevice d, VulkanShader* s);
void DestroyPipeline(VkDevice d, VulkanPipeline* p);






#endif
