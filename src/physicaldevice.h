#ifndef PHYSICAL_H
#define PHYSICAL_H
#include <stdint.h>
#include <vulkan/vulkan_core.h>
#include "Vulkan.h"
#include "error.h"
#include "optional.h"

DEF_OPTIONAL(uint32_t)

typedef struct QueueFamilyIndicies {
    optional(uint32_t) graphicsQueue;
    optional(uint32_t) presentQueue;
} QueueFamilyIndicies;


const static char* deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
const static uint32_t requiredDeviceExtensions = sizeof(deviceExtensions)/sizeof(char*);

static QueueFamilyIndicies findQueueFamily(VkPhysicalDevice* device, VkSurfaceKHR* surface) {
    QueueFamilyIndicies indicies;

    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(*device, &queueCount, NULL);

    VkQueueFamilyProperties properties[queueCount];
    vkGetPhysicalDeviceQueueFamilyProperties(*device, &queueCount, properties);
    

    VkBool32 graphicsSupport = VK_FALSE;
    VkBool32 presentSupport = VK_FALSE;
    for (uint32_t i = 0; i<queueCount; i++) {

        if ((!graphicsSupport) && (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
            indicies.graphicsQueue = (optional(uint32_t)){1, i};
            graphicsSupport = VK_TRUE;
        }

        vkGetPhysicalDeviceSurfaceSupportKHR(*device, i, *surface, &presentSupport);
        if (presentSupport) { 
            indicies.presentQueue = (optional(uint32_t)){1, i};
            presentSupport = VK_TRUE;
        }

        if (graphicsSupport && presentSupport) break;
    }


    return indicies;
}


static int isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR* surface){
    QueueFamilyIndicies indicies = findQueueFamily(&device, surface);
    
    uint32_t avalibleExtensionCount;

    vkEnumerateDeviceExtensionProperties(device, NULL, &avalibleExtensionCount, NULL);

    VkExtensionProperties avalibleExtensions[avalibleExtensionCount];
    vkEnumerateDeviceExtensionProperties(device, NULL, &avalibleExtensionCount, avalibleExtensions);

    uint32_t extensionCount = 0;
    for (int i = 0; i < avalibleExtensionCount; i++) {
        for (int j = 0; j < requiredDeviceExtensions; j++) {
            if (strcmp(avalibleExtensions[i].extensionName, deviceExtensions[j])) {
                extensionCount += 1;
            }
            if (extensionCount == requiredDeviceExtensions) break;
        }
        if (extensionCount == requiredDeviceExtensions) break;
    }

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, *surface, &formatCount, NULL);

    uint32_t presentCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, *surface, &presentCount, NULL);

    return (indicies.graphicsQueue.has_value && indicies.presentQueue.has_value) &&
           (extensionCount == requiredDeviceExtensions) &&
           (formatCount && presentCount);
}


static void pickPhysicalDevice(VkPhysicalDevice* p, VkInstance* instance, VkSurfaceKHR* surface) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(*instance, &deviceCount, NULL);

    if (deviceCount == 0) {
        errno = FailedSearch; 
        fprintf(stderr, ERR_COLOR("No Physical Device Found!"));
        return;
    }

    VkPhysicalDevice devices[deviceCount];
    vkEnumeratePhysicalDevices(*instance, &deviceCount, devices);
    
    int index = 0;
    for (int i = 0; i<deviceCount; i++) {
        if (isDeviceSuitable(devices[i], surface)) {
            *p = devices[i];
            index = i;
            break;
        }
    }

    if (*p == VK_NULL_HANDLE) {
        errno = FailedSearch;
        fprintf(stderr, ERR_COLOR("Failed to find a suitable Physical Device"));
        return;
    }
    fprintf(stdout, TRACE_COLOR("Physical Device %d Selected"), index);
    return;
}

#endif
