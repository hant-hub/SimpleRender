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


ErrorCode CreateSwapChain(RenderPass* r, SwapChain* s, VkSwapchainKHR old);
//ErrorCode CreateFrameBuffers(VulkanDevice* d, SwapChain*s, RenderPass* r);
ErrorCode CreateShaderProg(const char* vertex, const char* frag, VulkanShader* s);
ErrorCode CreatePipeline(VulkanShader* s, VulkanPipelineConfig* con, VulkanPipeline* p, RenderPass* r);

void DestroySwapChain(SwapChain* s);
void DestroyShaderProg(VulkanShader* s);
void DestroyPipeline(VulkanPipeline* p);






#endif
