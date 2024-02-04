#include "../init.h"
#include "error.h"
#include "log.h"
#include "util.h"
#include <string.h>
#include <vulkan/vulkan_core.h>

typedef struct {
    optional(uint32_t) graphicsFamily;
    optional(uint32_t) presentFamily;
} QueueFamilyIndicies;


ErrorCode querySwapDetails(SwapChainDetails* swapDetails, VkPhysicalDevice p, VkSurfaceKHR s) {
    vkGetPhysicalDeviceSurfaceFormatsKHR(p, s, &swapDetails->formatCount, NULL);
    swapDetails->formats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * swapDetails->formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(p, s, &swapDetails->formatCount, swapDetails->formats);
    
    vkGetPhysicalDeviceSurfacePresentModesKHR(p, s, &swapDetails->modeCount, NULL); 
    swapDetails->modes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * swapDetails->modeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(p, s, &swapDetails->modeCount, swapDetails->modes); 
     
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p, s, &swapDetails->capabilities);

    return SR_NO_ERROR;
}



static QueueFamilyIndicies findQueueFamilies(VkPhysicalDevice p, VkSurfaceKHR surface) {
    QueueFamilyIndicies indicies = {0};

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(p, &queueFamilyCount, NULL);
    VkQueueFamilyProperties families[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(p, &queueFamilyCount, families);

    for (int i = 0; i < queueFamilyCount; i++) {
        if (indicies.presentFamily.exist &&
            indicies.graphicsFamily.exist)
            break;


        VkBool32 pSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(p, i, surface, &pSupport);
        if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indicies.graphicsFamily.val = i;
                indicies.graphicsFamily.exist = TRUE;
        }

        if (pSupport) {
            indicies.presentFamily.val = i;
            indicies.presentFamily.exist = TRUE;
        }
    }

    return indicies;
}

static bool isDeviceSuitable(VkPhysicalDevice p, VkSurfaceKHR surface) {
    QueueFamilyIndicies indicies = findQueueFamilies(p, surface);

    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(p, NULL, &extensionCount, NULL); 
    VkExtensionProperties props[extensionCount];
    vkEnumerateDeviceExtensionProperties(p, NULL, &extensionCount, props); 
    
    uint32_t extensionsFound = 0;
    for (int i = 0; i < ARRAY_SIZE(props); ++i) {
        for (int j = 0; j < ARRAY_SIZE(deviceExtensions); ++j) {
            if (strcmp(props[i].extensionName, deviceExtensions[j])) {
                extensionsFound += 1;
            }
            if (extensionsFound == ARRAY_SIZE(deviceExtensions)) break;
        }
        if (extensionsFound == ARRAY_SIZE(deviceExtensions)) break;
    }


    bool AvalibleFormats = FALSE;
    SwapChainDetails details = {};
    querySwapDetails(&details, p, surface);
    if (details.modeCount && details.formatCount)
        AvalibleFormats = TRUE;

    free(details.modes);
    free(details.formats);
    
    return indicies.graphicsFamily.exist && 
           indicies.presentFamily.exist &&
           (extensionsFound == ARRAY_SIZE(deviceExtensions)) &&
           AvalibleFormats;
}




void DestroyDevice(VulkanDevice* d) {
    free(d->swapDetails.formats);
    free(d->swapDetails.modes);

    vkDestroyDevice(d->l, NULL);
    SR_LOG_DEB("Logical Device Destroyed");
}


ErrorCode CreateDevices(VulkanDevice* d, VulkanContext* context) {

    //Find Physical Device
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(context->instance, &deviceCount, NULL); 

#ifdef DEBUG
    if (deviceCount == 0) {
        SR_LOG_ERR("Failed to find any Vulkan Devices");
        return SR_DEVICE_NOT_FOUND;
    }
#endif
    VkPhysicalDevice devices[deviceCount];
    vkEnumeratePhysicalDevices(context->instance, &deviceCount, devices); 

    d->p = VK_NULL_HANDLE;
    for (int i = 0; i < deviceCount; i++) {
        if (isDeviceSuitable(devices[i], context->surface)) {
            d->p = devices[i];
            querySwapDetails(&d->swapDetails, d->p, context->surface);
#ifdef DEBUG
            printf("Device %i selected\n", i);
#endif
            break;
        }
    }

    if (d->p == VK_NULL_HANDLE) {
        SR_LOG_ERR("Failed to Find suitable Device");
        return SR_DEVICE_NOT_FOUND;
    }

    //Create Logical Device
    QueueFamilyIndicies indicies = findQueueFamilies(d->p, context->surface);    
    float priority = 1.0f;

    VkDeviceQueueCreateInfo graphInfo = {0};
    graphInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphInfo.queueFamilyIndex = indicies.graphicsFamily.val;
    graphInfo.queueCount = 1;
    graphInfo.pQueuePriorities = &priority;

    VkDeviceQueueCreateInfo presentInfo = {0};
    presentInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    presentInfo.queueFamilyIndex = indicies.presentFamily.val;
    presentInfo.queueCount = 1;
    presentInfo.pQueuePriorities = &priority;

    VkDeviceQueueCreateInfo queueInfos[] = {graphInfo, presentInfo};


    VkPhysicalDeviceFeatures deviceFeatures = {0};

    VkDeviceCreateInfo deviceInfo = {0};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pEnabledFeatures = &deviceFeatures;
    deviceInfo.pQueueCreateInfos = queueInfos;
    if (indicies.presentFamily.val != indicies.graphicsFamily.val) { 
        deviceInfo.queueCreateInfoCount = 2;
    } else {
        deviceInfo.queueCreateInfoCount = 1;
    }

    deviceInfo.enabledExtensionCount = ARRAY_SIZE(deviceExtensions);
    deviceInfo.ppEnabledExtensionNames = deviceExtensions;
#ifdef DEBUG
    deviceInfo.enabledLayerCount = ARRAY_SIZE(validationLayers);
    deviceInfo.ppEnabledLayerNames = validationLayers;
#else
    deviceInfo.enabledLayerCount = 0;
#endif

    if (vkCreateDevice(d->p, &deviceInfo, NULL, &d->l) != VK_SUCCESS) {
        SR_LOG_ERR("Logical Device Creation Failed");
        return SR_CREATE_FAIL;
    }
    SR_LOG_DEB("Logical Device Created");

    vkGetDeviceQueue(d->l, indicies.graphicsFamily.val, 0, &d->graph);
    vkGetDeviceQueue(d->l, indicies.presentFamily.val, 0, &d->present);
    return SR_NO_ERROR;
}
