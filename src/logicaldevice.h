#ifndef LOGICAL_DEVICE_H
#define LOGICAL_DEVICE_H
#include "Vulkan.h"
#include "instance.h"
#include "physicaldevice.h"
#include <stdint.h>
#include <vulkan/vulkan_core.h>


static void createLogicalDevice(VkDevice *d, VkQueue *graphicsQueue, VkQueue* presentQueue, VkPhysicalDevice* p, VkSurfaceKHR* surface, QueueFamilyIndicies* indicies) {
    *indicies = findQueueFamily(p, surface);
    uint32_t uniqueIndiciesCount = 2;
    if (indicies->graphicsQueue.value == indicies->presentQueue.value) {
        uniqueIndiciesCount = 1;
    }

    
    VkDeviceQueueCreateInfo queueCreateInfo[2] = {};

    float queuePriority = 1.0f;
    for (int i = 0; i < uniqueIndiciesCount; i++) {
        queueCreateInfo[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo[i].queueFamilyIndex = indicies->graphicsQueue.value;
        queueCreateInfo[i].queueCount = 1;
        queueCreateInfo[i].pQueuePriorities = &queuePriority;
    }


    VkPhysicalDeviceFeatures deviceFeatures = {};
    
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = uniqueIndiciesCount;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions;
    deviceCreateInfo.enabledExtensionCount = requiredDeviceExtensions;

    if (enableValidationLayers) {
        deviceCreateInfo.enabledLayerCount = validationLayerCount;
        deviceCreateInfo.ppEnabledLayerNames = validationLayers;
    } else {
        deviceCreateInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(*p, &deviceCreateInfo, NULL, d) != VK_SUCCESS) {
        errno = FailedCreation;
        fprintf(stderr, ERR_COLOR("Logical Device Creation Failed!"));
        return;
    }
    vkGetDeviceQueue(*d, indicies->graphicsQueue.value, 0, graphicsQueue);
    vkGetDeviceQueue(*d, indicies->presentQueue.value, 0, presentQueue);
    

    fprintf(stdout, TRACE_COLOR("Logical Device Creation Success"));
     

}







#endif
