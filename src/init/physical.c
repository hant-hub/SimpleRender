#include "../init.h"
#include "error.h"
#include "log.h"
#include "util.h"
#include <vulkan/vulkan_core.h>

typedef struct {
    optional(uint32_t) graphicsFamily;
} QueueFamilyIndicies;

static QueueFamilyIndicies findQueueFamilies(VkPhysicalDevice p) {
    QueueFamilyIndicies indicies;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(p, &queueFamilyCount, NULL);
    VkQueueFamilyProperties families[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(p, &queueFamilyCount, families);

    for (int i = 0; i < queueFamilyCount; i++) {
        if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indicies.graphicsFamily.val = i;
                indicies.graphicsFamily.exist = TRUE;
                break;
        }
    }

    return indicies;
}

static bool isDeviceSuitable(VkPhysicalDevice p) {
    QueueFamilyIndicies indicies = findQueueFamilies(p);
    return indicies.graphicsFamily.exist;
}




void DestroyDevice(VulkanDevice* d) {
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
        if (isDeviceSuitable(devices[i])) {
            d->p = devices[i];
            break;
        }
    }

    if (d->p == VK_NULL_HANDLE) {
        SR_LOG_ERR("Failed to Find suitable Device");
        return SR_DEVICE_NOT_FOUND;
    }

    //Create Logical Device
    QueueFamilyIndicies indicies = findQueueFamilies(d->p);    
    float priority = 1.0f;

    VkDeviceQueueCreateInfo queueInfo = {0};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueFamilyIndex = indicies.graphicsFamily.val;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &priority;


    VkPhysicalDeviceFeatures deviceFeatures = {0};

    VkDeviceCreateInfo deviceInfo = {0};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pEnabledFeatures = &deviceFeatures;

    deviceInfo.enabledExtensionCount = 0;
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
    return SR_NO_ERROR;
}
