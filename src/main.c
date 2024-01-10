#include "Vulkan.h"
#include "error.h"
#include "instance.h"
#include "optional.h"
#include "physicaldevice.h"
#include "logicaldevice.h"
#include "swapchain.h"
#include "imageviews.h"
#include "lib/SimpleMath/src/include/misc.h"
#include <GLFW/glfw3.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>



static const uint32_t WIDTH = 1920;
static const uint32_t HEIGHT = 1080;

typedef struct SizedBuffer {
    size_t size;
    char* buffer;
} SizedBuffer;

// Buffer is allocated with malloc, must be manually freed
static void LoadFile(const char* filename, SizedBuffer* buffer) {
    errno = 0;
    FILE* file = fopen(filename, "rb"); 
    if (file == NULL) {
        fprintf(stderr, ERR_COLOR("Failed to Open File"));
        return;
    }

    fseek(file, 0, SEEK_END);
    buffer->size = (size_t)ftell(file);
    fseek(file, 0, SEEK_SET); 

    buffer->buffer = (char *)aligned_alloc(sizeof(uint32_t), buffer->size*sizeof(char));


    if (buffer == NULL) {
        fprintf(stderr, ERR_COLOR("Malloc failed somehow. Fuck"));
        fclose(file);
        return;
    }

    if (fread(buffer->buffer, sizeof(char), buffer->size, file) != buffer->size) {
        fprintf(stderr, ERR_COLOR("Failed to Read entire file"));
        fclose(file);
        return;
    }

    if (errno != 0) {
        perror("readfile func");
    }

    fclose(file);
    return;
}




static void CreateSurface(VkInstance* instance, GLFWwindow* window, VkSurfaceKHR* surface) {
    if (glfwCreateWindowSurface(*instance, window, NULL, surface) != VK_SUCCESS) {
        errno = FailedCreation;
        fprintf(stderr, ERR_COLOR("Failed to Create Window Surface!"));
    }
    fprintf(stdout, TRACE_COLOR("Window Surface Created"));

}

inline static void CloseGLFW(GLFWwindow *w) {
    glfwDestroyWindow(w);
    glfwTerminate();
}

int main() {

    glfwInit();
    //create Window
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Renderer", glfwGetPrimaryMonitor(), NULL);

    //appinfo
    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Renderer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    //Create Instance
    VkInstance instance;
    CreateInstance(&instance, &appInfo);
    if (errno == FailedCreation) {
        CloseGLFW(window);
        exit(EXIT_FAILURE);
    }
    
    //Debug Messenging
    VkDebugUtilsMessengerEXT debugMessenger;
    CreateDebugMessenger(&debugMessenger, &instance);
    if (errno == FailedCreation) {
        CloseGLFW(window);
        vkDestroyInstance(instance, NULL);
        exit(EXIT_FAILURE);
    }
    

    //window surface creation
    VkSurfaceKHR surface;
    CreateSurface(&instance, window, &surface);
    if (errno == FailedCreation) {
        CloseGLFW(window); 
        if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);
        vkDestroyInstance(instance, NULL);
        exit(EXIT_FAILURE);
    }



    //Physical Device Creation
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    pickPhysicalDevice(&physicalDevice, &instance, &surface);
    if (errno == FailedSearch) {
        CloseGLFW(window);
        if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        exit(EXIT_FAILURE);
    }

    //Logical Device and Queue Creation
    VkDevice logicalDevice = VK_NULL_HANDLE; 
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
    createLogicalDevice(&logicalDevice, &graphicsQueue, &presentQueue, &physicalDevice, &surface);
    if (errno == FailedCreation) {
        CloseGLFW(window);
        if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);
        exit(EXIT_FAILURE);
    }


    //SwapChain Creation
    VkSwapchainKHR swapchain;
    SwapImageDetails details;
    CreateSwapChain(&swapchain, &details, physicalDevice, logicalDevice, surface, window);
    if (errno == FailedCreation) {
        CloseGLFW(window);
        if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyDevice(logicalDevice, NULL);
        vkDestroyInstance(instance, NULL);
        exit(EXIT_FAILURE);
    }

    uint32_t imageCount;
    vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, NULL); 
    VkImage swapChainImages[imageCount];
    vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, swapChainImages);

    //SwapChain Image Views
    VkImageView swapViews[imageCount];
    CreateImageViews(swapViews, swapChainImages, details, logicalDevice, imageCount);
    if (errno == FailedCreation) {
        CloseGLFW(window);
        if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);
        vkDestroySwapchainKHR(logicalDevice, swapchain, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyDevice(logicalDevice, NULL);
        vkDestroyInstance(instance, NULL);
        exit(EXIT_FAILURE);
    }


    //Load Shaders
    SizedBuffer vert; 
    SizedBuffer frag;
    errno = 0;
    LoadFile(".//shaders//standard.vert.spv", &vert);
    LoadFile(".//shaders//standard.frag.spv", &frag);

    if (errno != 0) {
        CloseGLFW(window);
        if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);
        DestroyImageViews(logicalDevice, swapViews, imageCount);
        vkDestroySwapchainKHR(logicalDevice, swapchain, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyDevice(logicalDevice, NULL);
        vkDestroyInstance(instance, NULL);
        exit(EXIT_FAILURE);
    }


    VkShaderModuleCreateInfo VertCreateInfo = {0};
    VertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    VertCreateInfo.codeSize = vert.size;
    VertCreateInfo.pCode = (uint32_t*)vert.buffer;

    VkShaderModule vertex;
    if (vkCreateShaderModule(logicalDevice, &VertCreateInfo, NULL, &vertex) != VK_SUCCESS) {
        free(vert.buffer);
        free(frag.buffer);

        CloseGLFW(window);
        if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);
        DestroyImageViews(logicalDevice, swapViews, imageCount);
        vkDestroySwapchainKHR(logicalDevice, swapchain, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyDevice(logicalDevice, NULL);
        vkDestroyInstance(instance, NULL);
        exit(EXIT_FAILURE);
    }
    free(vert.buffer);

    VkShaderModuleCreateInfo FragCreateInfo = {0};
    FragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    FragCreateInfo.codeSize = frag.size;
    FragCreateInfo.pCode = (uint32_t*)frag.buffer;

    VkShaderModule fragment;
    if (vkCreateShaderModule(logicalDevice, &FragCreateInfo, NULL, &fragment) != VK_SUCCESS) {
        free(frag.buffer);

        CloseGLFW(window);
        if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);
        DestroyImageViews(logicalDevice, swapViews, imageCount);
        vkDestroySwapchainKHR(logicalDevice, swapchain, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyDevice(logicalDevice, NULL);
        vkDestroyInstance(instance, NULL);
        exit(EXIT_FAILURE);
    }
    free(frag.buffer);

    vkDestroyShaderModule(logicalDevice, vertex, NULL);
    vkDestroyShaderModule(logicalDevice, fragment, NULL);

    

    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();


        glfwSwapBuffers(window);
    }

    if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);
    

    DestroyImageViews(logicalDevice, swapViews, imageCount);
    vkDestroySwapchainKHR(logicalDevice, swapchain, NULL);
    vkDestroySurfaceKHR(instance, surface, NULL);
    vkDestroyDevice(logicalDevice, NULL);
    vkDestroyInstance(instance, NULL);
    CloseGLFW(window);

    return EXIT_SUCCESS;
}
