#ifndef SR_PIPE_H
#define SR_PIPE_H

#include "error.h"
#include "config.h"
#include "error.h"
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
    RenderPass* rpass;
} SwapChain;


ErrorCode CreateSwapChain(RenderPass* r, SwapChain* s, SwapChain* old);
//ErrorCode CreateFrameBuffers(VulkanDevice* d, SwapChain*s, RenderPass* r);
ErrorCode CreateShaderProg(const char* vertex, const char* frag, VulkanShader* s);
ErrorCode CreatePipeline(VulkanShader* s, VulkanPipelineConfig* con, VulkanPipeline* p, RenderPass* r, u32 subpass);

void DestroySwapChain(SwapChain* s, RenderPass* r);
void DestroyShaderProg(VulkanShader* s);
void DestroyPipeline(VulkanPipeline* p);






#endif
