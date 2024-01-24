#ifndef INSTANCE_H
#define INSTANCE_H

#include "../Vulkan.h"
#include "error.h"
#include <vulkan/vulkan_core.h>

static const char* validationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};
static const uint32_t validationLayerCount = 1;

#ifdef NDEBUG
    static const int enableValidationLayers = 0;
#else
    static const int enableValidationLayers = 1;
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL debugcallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallBackData,
        void* pUserData) {
    if (messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        fprintf(stderr, TRACE_COLOR("Validation Layer: %s"), pCallBackData->pMessage);
    return VK_FALSE;
}

static int checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL); 

    VkLayerProperties properties[layerCount];
    vkEnumerateInstanceLayerProperties(&layerCount, properties);


    for (int i = 0; i < validationLayerCount; i++) {
        int layerfound = 0;

        for (int j = 0; j < layerCount; j++) {
            if (strcmp(validationLayers[i], properties[j].layerName) == 0) {
                layerfound = 1;
                break;
            }
        }

        if (!layerfound) return 0;
    }

    return 1;
}


static ErrorCode CreateDebugMessenger(VkDebugUtilsMessengerEXT* messenger, VkInstance* instance) {
    VkDebugUtilsMessengerCreateInfoEXT createInfo = (VkDebugUtilsMessengerCreateInfoEXT){};
    memset(&createInfo, 0, sizeof(VkDebugUtilsMessengerCreateInfoEXT));
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugcallback;
    createInfo.pUserData = NULL;

    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(*instance, "vkCreateDebugUtilsMessengerEXT");
    if (func == NULL) {
        fprintf(stderr, ERR_COLOR("Debug Messenger Creation Function Can't Be Found"));
        return Error;
    }
    
    if (func(*instance, &createInfo, NULL, messenger) != VK_SUCCESS) {
        fprintf(stderr, ERR_COLOR("Debug Messenger Creation Failed"));
        return Error;
    }

    return NoError;
}

static void DestroyDebugMessenger(VkInstance* instance, VkDebugUtilsMessengerEXT* messenger) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(*instance, "vkDestroyDebugUtilsMessengerEXT");
    
    if (func != NULL) {
        func(*instance, *messenger, NULL);
    }

}


static ErrorCode CreateInstance(VkInstance* out, VkApplicationInfo* app) {
    VkInstanceCreateInfo createInfo = {};
    memset(&createInfo, 0, sizeof(VkInstanceCreateInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = app;

    const char** RequiredExtensions;
    uint32_t count;

    RequiredExtensions = glfwGetRequiredInstanceExtensions(&count);
    const char** extensions = (const char**)calloc(sizeof(char*), ++count);  
    if (enableValidationLayers){ 
        memcpy(extensions, RequiredExtensions, (count-1)*sizeof(char*));
        extensions[count-1] = "VK_EXT_debug_utils";

        createInfo.ppEnabledExtensionNames = extensions;
        createInfo.enabledExtensionCount = count;
    } else {
        createInfo.ppEnabledExtensionNames = RequiredExtensions;
        createInfo.enabledExtensionCount = count;
    }


    if (enableValidationLayers) {
        if (!checkValidationLayerSupport()) {
            fprintf(stderr, ERR_COLOR("Requested Validation Layers Not Supported"));
            return Error;
        }

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
        memset(&debugCreateInfo, 0, sizeof(VkDebugUtilsMessengerCreateInfoEXT));
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debugcallback;
        debugCreateInfo.pUserData = NULL;

        createInfo.pNext = &debugCreateInfo;
        createInfo.enabledLayerCount = validationLayerCount;
        createInfo.ppEnabledLayerNames = validationLayers;
    } else {
        createInfo.enabledLayerCount = 0;
    }

    
    if (vkCreateInstance(&createInfo, NULL, out) != VK_SUCCESS) {
        fprintf(stderr, ERR_COLOR("Failed To Create Instance"));
        free(extensions);
        return Error;
    }

    fprintf(stdout, TRACE_COLOR("Instance Created"));  
    free(extensions);
    return NoError;
}



#endif
