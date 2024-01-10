#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H
#include "Vulkan.h"
#include "lib/SimpleMath/src/include/misc.h"
#include "physicaldevice.h"

typedef struct SwapImageDetails {
    VkExtent2D extent;
    VkFormat format;
} SwapImageDetails;

static void CreateSwapChain(VkSwapchainKHR* swapchain, SwapImageDetails* details, VkPhysicalDevice physicalDevice,
        VkDevice logicaldevice, VkSurfaceKHR surface, GLFWwindow* window) { 

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);
    VkSurfaceFormatKHR formats[formatCount];
    if (formatCount != 0) {
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats);
    }
    
    VkSurfaceFormatKHR format = formats[0];
    for (int i = 0; i < formatCount; i++) {
        if (formats[i].format == VK_FORMAT_B8G8R8_SRGB &&
            formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            format = formats[i];
        }
    }

    uint32_t presentCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentCount, NULL);
    VkPresentModeKHR presentmodes[presentCount];
    if (formatCount != 0) {
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentCount, presentmodes);
    }

    VkPresentModeKHR mode = VK_PRESENT_MODE_FIFO_KHR;    
    for (int i = 0; i < presentCount; i++) {
        if (presentmodes[i] == VK_PRESENT_MODE_MAILBOX_KHR) 
            mode = presentmodes[i];
    }

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
    VkExtent2D extent = {0, 0};
    
    if (capabilities.currentExtent.width != UINT32_MAX) {
        extent = capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        
        extent.width = (uint32_t)width;
        extent.height = (uint32_t)height;

        extent.width = clampf(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        extent.height = clampf(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = surface;

    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = format.format;
    swapchainCreateInfo.imageColorSpace = format.colorSpace;
    swapchainCreateInfo.imageExtent = extent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    details->extent = extent;
    details->format = format.format;

    QueueFamilyIndicies indicies = findQueueFamily(&physicalDevice, &surface); 
    uint32_t values[] = {indicies.graphicsQueue.value, indicies.presentQueue.value};
    if (values[0] != values[1]) {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = values;
    } else {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = NULL;
    }

    swapchainCreateInfo.preTransform = capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = mode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

   if (vkCreateSwapchainKHR(logicaldevice, &swapchainCreateInfo, NULL, swapchain) != VK_SUCCESS) {
        errno = FailedCreation;
        fprintf(stderr, ERR_COLOR("Swap Chain Creation Failed"));
    }
    fprintf(stdout, TRACE_COLOR("Swap Chain Created"));
}


#endif
