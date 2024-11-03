#include "init.h"
#include "error.h"
#include <string.h>
#include "stdlib.h"

static void ResizeCallback(GLFWwindow* window, int width, int height) {
    WIDTH = width;
    HEIGHT = height;
    frameBufferResized = TRUE;
}

uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;
bool frameBufferResized = FALSE;

VulkanContext sr_context = {0};
VulkanDevice sr_device = {0};

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

ErrorCode querySwapDetails(SwapChainDetails* swapDetails, VkPhysicalDevice p, VkSurfaceKHR s) {
    static SwapChainDetails cached = {0};
    static bool cached_flag = FALSE; 

    if (!cached_flag) {
        vkGetPhysicalDeviceSurfaceFormatsKHR(p, s, &swapDetails->formatCount, NULL);
        swapDetails->formats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * swapDetails->formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(p, s, &swapDetails->formatCount, swapDetails->formats);

        vkGetPhysicalDeviceSurfacePresentModesKHR(p, s, &swapDetails->modeCount, NULL); 
        swapDetails->modes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * swapDetails->modeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(p, s, &swapDetails->modeCount, swapDetails->modes); 

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p, s, &swapDetails->capabilities);
        cached = *swapDetails;
    } else {
        *swapDetails = cached; 
    }

    return SR_NO_ERROR;
}



QueueFamilyIndicies findQueueFamilies(VkPhysicalDevice p, VkSurfaceKHR surface) {
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
            if (strcmp(props[i].extensionName, deviceExtensions[j]) == 0) {
                extensionsFound += 1;
            }
            if (extensionsFound == ARRAY_SIZE(deviceExtensions)) break;
        }
        if (extensionsFound == ARRAY_SIZE(deviceExtensions)) break;
    }

    VkPhysicalDeviceFeatures supported;
    vkGetPhysicalDeviceFeatures(p, &supported);

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
           AvalibleFormats &&
           supported.samplerAnisotropy;
}

void DestroyVulkan() {
    DestroyCommand(&sr_context.cmd);
    vkDestroyDevice(sr_device.l, NULL);
    SR_LOG_DEB("Logical Device Destroyed");

    vkDestroySurfaceKHR(sr_context.instance, sr_context.surface, NULL);
    SR_LOG_DEB("Surface Destroyed");

#ifdef DEBUG
    DestroyDebugMessenger(&sr_context);
    SR_LOG_DEB("Debug Messenger Destroyed");
#endif
    vkDestroyInstance(sr_context.instance, NULL);
    SR_LOG_DEB("Instance Destroyed");

    glfwDestroyWindow(sr_context.w);
    glfwTerminate();

}



ErrorCode CreateVulkan() {

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "SimpleRender", NULL, NULL);
    glfwSetFramebufferSizeCallback(window, ResizeCallback);

    VulkanContext* context = &sr_context;
    context->w = window;

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

    if (glfwCreateWindowSurface(sr_context.instance, window, NULL, &sr_context.surface) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to create window Surface");
        return SR_CREATE_FAIL;
    }
    SR_LOG_DEB("Surface Created");

    VulkanDevice* d = &sr_device;

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
#ifdef DEBUG
            SR_LOG_DEB("Device %i selected", i);
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
    deviceFeatures.samplerAnisotropy = VK_TRUE;

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
    d->indicies = indicies;

    CreateCommand(&sr_context.cmd);

    return SR_NO_ERROR;
}
