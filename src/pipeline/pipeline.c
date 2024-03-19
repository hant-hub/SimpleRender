#include "../pipeline.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include "util.h"
#include "vertex.h"
#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>
#include <vulkan/vulkan_core.h>

ErrorCode CreatePipelineConfig(VulkanDevice* d, VulkanContext* c, VkFormat swapFormat, VulkanShader* s, VulkanPipelineConfig* p) {
    //Shader stage creation
    p->stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    p->stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    p->stages[0].module = s->vertex;
    p->stages[0].pName = "main";

    p->stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    p->stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    p->stages[1].module = s->fragment;
    p->stages[1].pName = "main";


    p->states[0] = VK_DYNAMIC_STATE_VIEWPORT;
    p->states[1] = VK_DYNAMIC_STATE_SCISSOR;

    VkPipelineDynamicStateCreateInfo dynamicInfo = {0};
    dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicInfo.dynamicStateCount = ARRAY_SIZE(p->states);
    dynamicInfo.pDynamicStates = p->states;
    p->dynamic = dynamicInfo;


    VkPipelineVertexInputStateCreateInfo vertInputInfo = {0};
    vertInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertInputInfo.vertexBindingDescriptionCount = 1;
    vertInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertInputInfo.vertexAttributeDescriptionCount = 2;
    vertInputInfo.pVertexAttributeDescriptions = attrDescription;
    p->vertInput = vertInputInfo;

    VkPipelineInputAssemblyStateCreateInfo assemInfo = {0};
    assemInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assemInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    assemInfo.primitiveRestartEnable = VK_FALSE;
    p->assembly = assemInfo;

    VkPipelineViewportStateCreateInfo viewPortState = {0};
    viewPortState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewPortState.viewportCount = 1;
    viewPortState.scissorCount = 1;
    p->view = viewPortState;

    VkPipelineRasterizationStateCreateInfo rasterInfo = {0};
    rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterInfo.depthClampEnable = VK_FALSE;
    rasterInfo.rasterizerDiscardEnable = VK_FALSE;
    

    rasterInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterInfo.lineWidth = 1.0f;
    rasterInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;

    rasterInfo.depthBiasEnable = VK_FALSE;
    rasterInfo.depthBiasConstantFactor = 0.0f;
    rasterInfo.depthBiasClamp = 0.0f;
    rasterInfo.depthBiasSlopeFactor = 0.0f;
    p->raster = rasterInfo;
    
    VkPipelineMultisampleStateCreateInfo multiInfo = {0};
    multiInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multiInfo.sampleShadingEnable = VK_FALSE;
    multiInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multiInfo.minSampleShading = 1.0f; 
    multiInfo.pSampleMask = NULL;
    multiInfo.alphaToCoverageEnable = VK_FALSE;
    multiInfo.alphaToOneEnable = VK_FALSE;
    p->multisample = multiInfo;

    VkPipelineColorBlendAttachmentState blendInfo = {0};
    blendInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    blendInfo.blendEnable = VK_FALSE;
    blendInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    blendInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendInfo.colorBlendOp = VK_BLEND_OP_ADD;
    blendInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    blendInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendInfo.alphaBlendOp = VK_BLEND_OP_ADD;
    p->colorattachment = blendInfo;

    VkPipelineColorBlendStateCreateInfo blendStateInfo = {0};
    blendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blendStateInfo.logicOpEnable = VK_FALSE;
    blendStateInfo.logicOp = VK_LOGIC_OP_COPY;
    blendStateInfo.attachmentCount = 1;
    blendStateInfo.pAttachments = &p->colorattachment;
    blendStateInfo.blendConstants[0] = 0.0f;
    blendStateInfo.blendConstants[1] = 0.0f;
    blendStateInfo.blendConstants[2] = 0.0f;
    blendStateInfo.blendConstants[3] = 0.0f;
    p->colorState = blendStateInfo;
    
    VkDescriptorSetLayoutBinding uboLayoutBinding = {0};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;

    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = NULL;

    VkDescriptorSetLayoutCreateInfo descriptorInfo = {0};
    descriptorInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorInfo.bindingCount = 1;
    descriptorInfo.pBindings = &uboLayoutBinding;


    if (vkCreateDescriptorSetLayout(d->l, &descriptorInfo, NULL, &p->descriptorLayout) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Create Descriptor Layout");
        return SR_CREATE_FAIL;
    }

    VkDescriptorPoolSize poolSize = {0};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = (uint32_t) SR_MAX_FRAMES_IN_FLIGHT;

    VkDescriptorPoolCreateInfo poolInfo = {0};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;

    poolInfo.maxSets = (uint32_t) SR_MAX_FRAMES_IN_FLIGHT;

    if (vkCreateDescriptorPool(d->l, &poolInfo, NULL, &p->descriptorPool) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Create Descriptor Pool");
        return SR_CREATE_FAIL;
    }

    VkDescriptorSetLayout layouts[] = {
        p->descriptorLayout,
        p->descriptorLayout,
        p->descriptorLayout
    };
    VkDescriptorSetAllocateInfo descpAlloc = {0};
    descpAlloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descpAlloc.descriptorPool = p->descriptorPool;
    descpAlloc.descriptorSetCount = (uint32_t) SR_MAX_FRAMES_IN_FLIGHT;
    descpAlloc.pSetLayouts = layouts;

    if (vkAllocateDescriptorSets(d->l, &descpAlloc, p->descriptorSet) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Create Descriptor Set");
        return SR_CREATE_FAIL;
    }


    VkPipelineLayoutCreateInfo layoutInfo = {0};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = &p->descriptorLayout;
    layoutInfo.pushConstantRangeCount = 0;
    layoutInfo.pPushConstantRanges = NULL;

    if (vkCreatePipelineLayout(d->l, &layoutInfo, NULL, &p->layout) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Create Pipeline Layout");
        return SR_CREATE_FAIL;
    }



    //Make RenderPass
    VkAttachmentDescription colorAttachment = {0};
    colorAttachment.format = swapFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachRef = {0};
    colorAttachRef.attachment = 0;
    colorAttachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subDescription = {0};
    subDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subDescription.colorAttachmentCount = 1;
    subDescription.pColorAttachments = &colorAttachRef;

    VkSubpassDependency dependency = {0};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    
    VkRenderPassCreateInfo renderInfo = {0};
    renderInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderInfo.attachmentCount = 1;
    renderInfo.pAttachments = &colorAttachment;
    renderInfo.subpassCount = 1;
    renderInfo.pSubpasses = &subDescription;
    renderInfo.dependencyCount = 1;
    renderInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(d->l, &renderInfo, NULL, &p->pass) != VK_SUCCESS) {
        SR_LOG_WAR("Failed to Create Render Pass");
        return SR_CREATE_FAIL;
    }

    SR_LOG_DEB("Pipeline Config Created");
    return SR_NO_ERROR;
}

ErrorCode CreatePipeline(VulkanDevice* d, VulkanContext* c, VulkanShader* s, VulkanPipelineConfig* con, VulkanPipeline* p) {
    VkGraphicsPipelineCreateInfo pipelineInfo = {0};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = con->stages;

    pipelineInfo.pVertexInputState = &con->vertInput;
    pipelineInfo.pInputAssemblyState = &con->assembly;
    pipelineInfo.pViewportState = &con->view;
    pipelineInfo.pRasterizationState = &con->raster;
    pipelineInfo.pMultisampleState = &con->multisample;
    pipelineInfo.pDepthStencilState = NULL;
    pipelineInfo.pColorBlendState = &con->colorState;
    pipelineInfo.pDynamicState = &con->dynamic;

    pipelineInfo.layout = con->layout;
    pipelineInfo.renderPass = con->pass;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(d->l, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &p->pipeline) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Create Graphics Pipeline");
        return SR_CREATE_FAIL;
    }

    p->pass = con->pass;
    SR_LOG_DEB("Graphics Pipeline Created");
    return SR_NO_ERROR;
}

void DestroyShaderProg(VkDevice d, VulkanShader* s) {
    vkDestroyShaderModule(d, s->vertex, NULL);
    vkDestroyShaderModule(d, s->fragment, NULL);
}

void DestroyPipelineConfig(VkDevice d, VulkanPipelineConfig* p) {
    vkDestroyDescriptorPool(d, p->descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(d, p->descriptorLayout, NULL);
    vkDestroyPipelineLayout(d, p->layout, NULL);
    vkDestroyRenderPass(d, p->pass, NULL);
    SR_LOG_DEB("Pipeline Config Destroyed");
}

void DestroyPipeline(VkDevice d, VulkanPipeline* p) {
    vkDestroyPipeline(d, p->pipeline, NULL);
    SR_LOG_DEB("Graphics Pipeline Destroyed");
}

