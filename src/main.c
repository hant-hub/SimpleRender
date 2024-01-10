#include "Vulkan.h"
#include "error.h"
#include "util.h"
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

typedef struct PipelineData {
    VkViewport view;
    VkRect2D scissor;
    VkPipelineLayout layout;
} PipelineData;


static void CreateGraphicsPipeline(VkDevice logicalDevice, SwapImageDetails swapChainExtent, PipelineData* pipeData, GLFWwindow* window) {
    //Load Shaders
    SizedBuffer vert; 
    SizedBuffer frag;
    errno = 0;
    LoadFile(".//shaders//standard.vert.spv", &vert);
    LoadFile(".//shaders//standard.frag.spv", &frag);

    if (errno != 0) {
        errno = FailedSearch;
        fprintf(stderr, ERR_COLOR("Failed to Load File"));
        return;
    }


    VkShaderModuleCreateInfo VertCreateInfo = {0};
    VertCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    VertCreateInfo.codeSize = vert.size;
    VertCreateInfo.pCode = (uint32_t*)vert.buffer;

    VkShaderModule vertex;
    if (vkCreateShaderModule(logicalDevice, &VertCreateInfo, NULL, &vertex) != VK_SUCCESS) {
        free(vert.buffer);
        free(frag.buffer);
        errno = FailedCreation;
        fprintf(stderr, ERR_COLOR("Failed to Create Vertex Shader Module"));
        return;
    }
    free(vert.buffer);

    VkShaderModuleCreateInfo FragCreateInfo = {0};
    FragCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    FragCreateInfo.codeSize = frag.size;
    FragCreateInfo.pCode = (uint32_t*)frag.buffer;

    VkShaderModule fragment;
    if (vkCreateShaderModule(logicalDevice, &FragCreateInfo, NULL, &fragment) != VK_SUCCESS) {
        free(frag.buffer);
        vkDestroyShaderModule(logicalDevice, vertex, NULL);
        errno = FailedCreation;
        fprintf(stderr, ERR_COLOR("Failed to Create Fragment Shader Module"));
        return;

    }
    free(frag.buffer);

    VkPipelineShaderStageCreateInfo vertStageInfo = {0};
    vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStageInfo.module = vertex;
    vertStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragStageInfo = {0};
    fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStageInfo.module = fragment;
    fragStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertStageInfo, fragStageInfo};

    //fixed shader stages
    const VkDynamicState dynamicStates[] = {
//        VK_DYNAMIC_STATE_VIEWPORT,
//        VK_DYNAMIC_STATE_SCISSOR,
    };
    
    VkPipelineDynamicStateCreateInfo dynamicCreateInfo = {0};
    dynamicCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicCreateInfo.dynamicStateCount = sizeof(dynamicStates)/sizeof(dynamicStates[0]); 
    dynamicCreateInfo.pDynamicStates = dynamicStates;


    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = NULL;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = NULL;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {0};
    inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;


    VkViewport viewport = {0};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainExtent.extent.width;
    viewport.height = (float)swapChainExtent.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {0};
    scissor.offset = (VkOffset2D){0,0};
    scissor.extent = swapChainExtent.extent;

    VkPipelineViewportStateCreateInfo viewportInfo = {0};
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.viewportCount = 1;
    viewportInfo.pViewports = &viewport;
    viewportInfo.scissorCount = 1;
    viewportInfo.pScissors = &scissor;


    VkPipelineRasterizationStateCreateInfo rasterizerInfo = {0};
    rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerInfo.depthClampEnable = VK_FALSE;
    rasterizerInfo.rasterizerDiscardEnable = VK_TRUE;
    rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizerInfo.lineWidth = 1.0f;
    rasterizerInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizerInfo.depthBiasEnable = VK_FALSE;
    rasterizerInfo.depthBiasConstantFactor = 0.0f;
    rasterizerInfo.depthBiasClamp = 0.0f;
    rasterizerInfo.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisamplingInfo = {0};
    multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisamplingInfo.sampleShadingEnable = VK_FALSE;
    multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisamplingInfo.minSampleShading = 1.0f; 
    multisamplingInfo.pSampleMask = NULL; 
    multisamplingInfo.alphaToCoverageEnable = VK_FALSE; 
    multisamplingInfo.alphaToOneEnable = VK_FALSE; 


    VkPipelineColorBlendAttachmentState colorBlendInfo = {0};
    colorBlendInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendInfo.blendEnable = VK_FALSE;
    colorBlendInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendInfo.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendInfo.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
    
    VkPipelineColorBlendStateCreateInfo colorStateInfo = {0};
    colorStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorStateInfo.logicOpEnable = VK_FALSE;
    colorStateInfo.logicOp = VK_LOGIC_OP_COPY; 
    colorStateInfo.attachmentCount = 1;
    colorStateInfo.pAttachments = &colorBlendInfo;
    colorStateInfo.blendConstants[0] = 0.0f; 
    colorStateInfo.blendConstants[1] = 0.0f; 
    colorStateInfo.blendConstants[2] = 0.0f; 
    colorStateInfo.blendConstants[3] = 0.0f; 

    VkPipelineLayoutCreateInfo layoutInfo = {0};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 0; 
    layoutInfo.pSetLayouts = NULL; 
    layoutInfo.pushConstantRangeCount = 0; 
    layoutInfo.pPushConstantRanges = NULL; 

    if (vkCreatePipelineLayout(logicalDevice, &layoutInfo, NULL, &pipeData->layout) != VK_SUCCESS) {
        errno = FailedCreation;
        fprintf(stderr, ERR_COLOR("Failed to create Pipeline Layout"));
        vkDestroyShaderModule(logicalDevice, vertex, NULL);
        vkDestroyShaderModule(logicalDevice, fragment, NULL);
        return;
    }
    fprintf(stdout, TRACE_COLOR("\tPipeline Layout Created"));



    vkDestroyShaderModule(logicalDevice, vertex, NULL);
    vkDestroyShaderModule(logicalDevice, fragment, NULL);
    fprintf(stdout, TRACE_COLOR("Created Graphics Pipeline"));
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

    //create Graphics Pipeline
    PipelineData data;
    CreateGraphicsPipeline(logicalDevice, details, &data, window);
    if (errno == FailedCreation) {
        CloseGLFW(window);
        if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);
        DestroyImageViews(logicalDevice, swapViews, imageCount);
        vkDestroySwapchainKHR(logicalDevice, swapchain, NULL);
        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyDevice(logicalDevice, NULL);
        vkDestroyInstance(instance, NULL);
        exit(EXIT_FAILURE);
    }


    

    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();


        glfwSwapBuffers(window);
    }

    if (enableValidationLayers)  DestroyDebugMessenger(&instance, &debugMessenger);
    

    vkDestroyPipelineLayout(logicalDevice, data.layout, NULL);
    DestroyImageViews(logicalDevice, swapViews, imageCount);
    vkDestroySwapchainKHR(logicalDevice, swapchain, NULL);
    vkDestroySurfaceKHR(instance, surface, NULL);
    vkDestroyDevice(logicalDevice, NULL);
    vkDestroyInstance(instance, NULL);
    CloseGLFW(window);

    return EXIT_SUCCESS;
}
