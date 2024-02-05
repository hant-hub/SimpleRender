#ifndef SR_SWAP_H
#define SR_SWAP_H
#include "error.h"
#include "init.h"
#include "log.h"
#include "error.h"
#include "util.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

typedef struct {
    VkSurfaceFormatKHR format;
    VkPresentModeKHR mode;
    VkExtent2D extent;
    uint32_t imgCount;
    VkSwapchainKHR swapChain;
    VkImageView* views;
} SwapChain;


void DestroySwapChain(VkDevice l, SwapChain* s);
ErrorCode CreateSwapChain(VulkanDevice* d, VulkanContext* c, SwapChain* s, VkSwapchainKHR old);




#endif
