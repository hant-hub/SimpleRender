#include "../init.h"
#include "error.h"
#include "log.h"
#include <GLFW/glfw3.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>
#include <string.h>
#include <stdlib.h>

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;
bool frameBufferResized = FALSE;

static bool CheckValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    VkLayerProperties props[layerCount];
    vkEnumerateInstanceLayerProperties(&layerCount, props);

    for (size_t i = 0; i < ARRAY_SIZE(validationLayers); i++) {
        bool layerfound = FALSE;

        for (size_t j = 0; j < layerCount; j++) {
            if (strcmp(validationLayers[i], props[j].layerName) == 0) {
                layerfound = TRUE;
                break;
            }
        }

        if (!layerfound) {
            return FALSE;
        }
    }
    return TRUE;
}
#ifdef DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT flags,
        const VkDebugUtilsMessengerCallbackDataEXT* callData,
        void* pUserData) {

    if (severity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        fprintf(stdout, "Validation Layer: %s", callData->pMessage);

    return VK_FALSE;
}


static ErrorCode CreateDebugMessenger(VkDebugUtilsMessengerEXT* messenger, VkInstance* instance) {
    VkDebugUtilsMessengerCreateInfoEXT createInfo = (VkDebugUtilsMessengerCreateInfoEXT){0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = NULL;

    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(*instance, "vkCreateDebugUtilsMessengerEXT");
    if (func == NULL) {
        SR_LOG_ERR("Could not find Debug Messenger Create Function");
        return SR_EXTENSION_MISSING;
    }
    
    if (func(*instance, &createInfo, NULL, messenger) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Create Debug Messenger");
        return SR_CREATE_FAIL;
    }
    SR_LOG_DEB("Created Debug Messenger");
    return SR_NO_ERROR;
}

void DestroyDebugMessenger(VulkanContext* context) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(context->instance, "vkDestroyDebugUtilsMessengerEXT");
    
    if (func != NULL) {
        func(context->instance, context->debug, NULL);
    }

}

#endif


void DestroyContext(VulkanContext* context) {
    vkDestroySurfaceKHR(context->instance, context->surface, NULL);
    SR_LOG_DEB("Surface Destroyed");

#ifdef DEBUG
    DestroyDebugMessenger(context);
    SR_LOG_DEB("Debug Messenger Destroyed");
#endif
    vkDestroyInstance(context->instance, NULL);
    SR_LOG_DEB("Instance Destroyed");


}



ErrorCode CreateInstance(VulkanContext* context) {

#ifdef DEBUG
    if (!CheckValidationLayerSupport()) {
        SR_LOG_ERR("Validation Layers Required but not Found");
        return SR_VALIDATION_MISSING; 
    }

#endif


    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "SimpleRender";
    appInfo.applicationVersion = VK_MAKE_VERSION(2, 0, 0);
    appInfo.pEngineName = "SimpleRender";
    appInfo.engineVersion = VK_MAKE_VERSION(2, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;


    VkInstanceCreateInfo instanceInfo = {0};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char ** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
#ifdef DEBUG

    const char * extensions[glfwExtensionCount + 1];
    memcpy(extensions, glfwExtensions, (glfwExtensionCount)*sizeof(char*));
    extensions[glfwExtensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    uint32_t extensionCount = glfwExtensionCount + 1;

#else
    const char ** extensions = glfwExtensions;
    uint32_t extensionCount = glfwExtensionCount;
#endif

    instanceInfo.enabledExtensionCount = extensionCount;
    instanceInfo.ppEnabledExtensionNames = extensions;


#ifdef DEBUG
    instanceInfo.enabledLayerCount = ARRAY_SIZE(validationLayers);
    instanceInfo.ppEnabledLayerNames = validationLayers;


    VkDebugUtilsMessengerCreateInfoEXT debugInfo = (VkDebugUtilsMessengerCreateInfoEXT){0};
    debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugInfo.pfnUserCallback = debugCallback;
    debugInfo.pUserData = NULL;

    instanceInfo.pNext = &debugInfo;

#else
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.pNext = NULL;
#endif


    
    VkResult result = vkCreateInstance(&instanceInfo, NULL, &context->instance);
    switch(result) {
        case VK_SUCCESS:
            SR_LOG_DEB("Vulkan Instance Created");
            break;
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            SR_LOG_ERR("Missing Vulkan Extensions");
            return SR_EXTENSION_MISSING;
        default:
            SR_LOG_ERR("Failed to Create Instance");
            return SR_CREATE_FAIL;
    }

#ifdef DEBUG
    if (CreateDebugMessenger(&context->debug, &context->instance) != VK_SUCCESS) {
        return SR_CREATE_FAIL;
    }
#endif

    return SR_NO_ERROR;
}


ErrorCode CreateSurface(VulkanContext* context, GLFWwindow* window) {
    if (glfwCreateWindowSurface(context->instance, window, NULL, &context->surface) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to create window Surface");
        return SR_CREATE_FAIL;
    }
    SR_LOG_DEB("Surface Created");

    return SR_NO_ERROR;
}
