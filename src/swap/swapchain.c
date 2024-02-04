#include "../swap.h"
#include "error.h"
#include "init.h"
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>
#include "SimpleMath/src/include/misc.h"


ErrorCode CreateSwapChain(VulkanDevice* d, VulkanContext* c, SwapChain* s) {
    //Pick Format
    s->format = d->swapDetails.formats[0];
    for (int i = 0; i < d->swapDetails.formatCount; i++) {
        if (d->swapDetails.formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && 
            d->swapDetails.formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            s->format = d->swapDetails.formats[i];
            break;
        }
    }

    //Pick Mode
    s->mode = VK_PRESENT_MODE_FIFO_KHR;

    //pick Extent
    VkSurfaceCapabilitiesKHR capabilities = d->swapDetails.capabilities;
    if (capabilities.currentExtent.width != UINT32_MAX) {
        s->extent = capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(c->w, &width, &height);

        s->extent.width = ;
        s->extent.height = height;
    }

    
    return SR_NO_ERROR;
}

