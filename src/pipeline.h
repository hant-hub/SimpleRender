#ifndef PIPELINE_H
#define PIPELINE_H
#include "Vulkan.h"
#include "swapchain.h"
#include "util.h"

typedef struct PipelineData {
    VkViewport view;
    VkRect2D scissor;
    VkPipelineLayout layout;
} PipelineData;


static void CreateRenderPass(VkRenderPass* renderPass, VkDevice logicalDevice, SwapImageDetails* details) {
    VkAttachmentDescription renderColorAttachment = {};
    renderColorAttachment.format = details->format;
    renderColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    renderColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    renderColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    renderColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    renderColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    renderColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    renderColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference renderColorReference = {};
    renderColorReference.attachment = 0;
    renderColorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription renderSubpass = {};
    renderSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    renderSubpass.colorAttachmentCount = 1;
    renderSubpass.pColorAttachments = &renderColorReference;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &renderColorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &renderSubpass;

    if (vkCreateRenderPass(logicalDevice, &renderPassInfo, NULL, renderPass) != VK_SUCCESS) {
        errno = FailedCreation;
        fprintf(stderr, ERR_COLOR("Failed to Create Render Pass"));
        return;
    }

    fprintf(stdout, TRACE_COLOR("Render Pass Created"));
}





static void CreateGraphicsPipeline(VkPipeline* pipeline, VkRenderPass renderPass, VkDevice logicalDevice, SwapImageDetails swapChainExtent, PipelineData* pipeData, GLFWwindow* window) {
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


    VkShaderModuleCreateInfo VertCreateInfo = {};
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

    VkShaderModuleCreateInfo FragCreateInfo = {};
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

    VkPipelineShaderStageCreateInfo vertStageInfo = {};
    vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStageInfo.module = vertex;
    vertStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragStageInfo = {};
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
    
    VkPipelineDynamicStateCreateInfo dynamicCreateInfo = {};
    dynamicCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicCreateInfo.dynamicStateCount = sizeof(dynamicStates)/sizeof(dynamicStates[0]); 
    dynamicCreateInfo.pDynamicStates = dynamicStates;


    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = NULL;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = NULL;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
    inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;


    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainExtent.extent.width;
    viewport.height = (float)swapChainExtent.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = (VkOffset2D){0,0};
    scissor.extent = swapChainExtent.extent;

    VkPipelineViewportStateCreateInfo viewportInfo = {};
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.viewportCount = 1;
    viewportInfo.pViewports = &viewport;
    viewportInfo.scissorCount = 1;
    viewportInfo.pScissors = &scissor;


    VkPipelineRasterizationStateCreateInfo rasterizerInfo = {};
    rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerInfo.depthClampEnable = VK_FALSE;
    rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizerInfo.lineWidth = 1.0f;
    rasterizerInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizerInfo.depthBiasEnable = VK_FALSE;
    rasterizerInfo.depthBiasConstantFactor = 0.0f;
    rasterizerInfo.depthBiasClamp = 0.0f;
    rasterizerInfo.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisamplingInfo = {};
    multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisamplingInfo.sampleShadingEnable = VK_FALSE;
    multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisamplingInfo.minSampleShading = 1.0f; 
    multisamplingInfo.pSampleMask = NULL; 
    multisamplingInfo.alphaToCoverageEnable = VK_FALSE; 
    multisamplingInfo.alphaToOneEnable = VK_FALSE; 


    VkPipelineColorBlendAttachmentState colorBlendInfo = {};
    colorBlendInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendInfo.blendEnable = VK_FALSE;
    colorBlendInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendInfo.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendInfo.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
    
    VkPipelineColorBlendStateCreateInfo colorStateInfo = {};
    colorStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorStateInfo.logicOpEnable = VK_FALSE;
    colorStateInfo.logicOp = VK_LOGIC_OP_COPY; 
    colorStateInfo.attachmentCount = 1;
    colorStateInfo.pAttachments = &colorBlendInfo;
    colorStateInfo.blendConstants[0] = 0.0f; 
    colorStateInfo.blendConstants[1] = 0.0f; 
    colorStateInfo.blendConstants[2] = 0.0f; 
    colorStateInfo.blendConstants[3] = 0.0f; 

    VkPipelineLayoutCreateInfo layoutInfo = {};
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

    //Graphics Pipeline Creation
    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderStages;
    
    pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyInfo;
    pipelineCreateInfo.pViewportState = &viewportInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizerInfo;
    pipelineCreateInfo.pMultisampleState = &multisamplingInfo;
    pipelineCreateInfo.pDepthStencilState = NULL;
    pipelineCreateInfo.pColorBlendState = &colorStateInfo;
    pipelineCreateInfo.pDynamicState = &dynamicCreateInfo;

    pipelineCreateInfo.layout = pipeData->layout;
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = 0;

    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, NULL, pipeline) != VK_SUCCESS) {
        errno = FailedCreation;
        fprintf(stderr, ERR_COLOR("Failed to create Graphics Pipeline"));
        vkDestroyShaderModule(logicalDevice, vertex, NULL);
        vkDestroyShaderModule(logicalDevice, fragment, NULL);
        return;
    }


    vkDestroyShaderModule(logicalDevice, vertex, NULL);
    vkDestroyShaderModule(logicalDevice, fragment, NULL);
    fprintf(stdout, TRACE_COLOR("Created Graphics Pipeline"));
}

#endif
