#include "config.h"
#include "init.h"
#include <vulkan/vulkan_core.h>


ErrorCode CreatePipelineConfig(VulkanShader* s, VulkanConfigInput v, VulkanPipelineConfig* p, bool depthEnable) {

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

    VkPipelineDepthStencilStateCreateInfo depthStencil = {0};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    if (depthEnable) {
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f;
        depthStencil.maxDepthBounds = 1.0f;

        depthStencil.stencilTestEnable = VK_TRUE;
        depthStencil.front = (VkStencilOpState){};
        depthStencil.back = (VkStencilOpState){};
    } else {
        depthStencil.depthTestEnable = VK_FALSE;
        depthStencil.depthWriteEnable = VK_FALSE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_ALWAYS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f;
        depthStencil.maxDepthBounds = 1.0f;

        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = (VkStencilOpState){};
        depthStencil.back = (VkStencilOpState){};
    }
    p->depth = depthStencil;

    VkPipelineVertexInputStateCreateInfo vertInputInfo = {0};
    vertInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertInputInfo.vertexBindingDescriptionCount = v.bsize;
    vertInputInfo.pVertexBindingDescriptions = v.bindings;
    vertInputInfo.vertexAttributeDescriptionCount = v.asize;
    vertInputInfo.pVertexAttributeDescriptions = v.attrs;
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
    blendInfo.blendEnable = VK_TRUE;
    blendInfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
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

//    VkDescriptorSetLayoutBinding layoutBindings[2] = {0}; 
//    
//    layoutBindings[0].binding = 0;
//    layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//    layoutBindings[0].descriptorCount = 1;
//
//    layoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//    layoutBindings[0].pImmutableSamplers = NULL;
//
//    layoutBindings[1].binding = 1;
//    layoutBindings[1].descriptorCount = 1;
//    layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//    layoutBindings[1].pImmutableSamplers = NULL;
//    layoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
//
//
//    VkDescriptorSetLayoutCreateInfo descriptorInfo = {0};
//    descriptorInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//    descriptorInfo.bindingCount = 2;
//    descriptorInfo.pBindings = layoutBindings;
//
//
//    if (vkCreateDescriptorSetLayout(d->l, &descriptorInfo, NULL, &p->descrip.descriptorLayout) != VK_SUCCESS) {
//        SR_LOG_ERR("Failed to Create Descriptor Layout");
//        return SR_CREATE_FAIL;
//    }
//
//    VkDescriptorPoolSize poolSize[2] = {0};
//    poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//    poolSize[0].descriptorCount = (uint32_t) SR_MAX_FRAMES_IN_FLIGHT;
//    poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//    poolSize[1].descriptorCount = (uint32_t) SR_MAX_FRAMES_IN_FLIGHT;
//
//    VkDescriptorPoolCreateInfo poolInfo = {0};
//    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//    poolInfo.poolSizeCount = 2;
//    poolInfo.pPoolSizes = poolSize;
//
//    poolInfo.maxSets = (uint32_t) SR_MAX_FRAMES_IN_FLIGHT;
//
//    if (vkCreateDescriptorPool(d->l, &poolInfo, NULL, &p->descrip.descriptorPool) != VK_SUCCESS) {
//        SR_LOG_ERR("Failed to Create Descriptor Pool");
//        return SR_CREATE_FAIL;
//    }
//
//    VkDescriptorSetLayout layouts[] = {
//        p->descrip.descriptorLayout,
//        p->descrip.descriptorLayout,
//        p->descrip.descriptorLayout
//    };
//    VkDescriptorSetAllocateInfo descpAlloc = {0};
//    descpAlloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//    descpAlloc.descriptorPool = p->descrip.descriptorPool;
//    descpAlloc.descriptorSetCount = (uint32_t) SR_MAX_FRAMES_IN_FLIGHT;
//    descpAlloc.pSetLayouts = layouts;
//
//    if (vkAllocateDescriptorSets(d->l, &descpAlloc, p->descrip.descriptorSet) != VK_SUCCESS) {
//        SR_LOG_ERR("Failed to Create Descriptor Set");
//        return SR_CREATE_FAIL;
//    }


    VkPipelineLayoutCreateInfo layoutInfo = {0};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = &p->descrip.descriptorLayout;
    layoutInfo.pushConstantRangeCount = 0;
    layoutInfo.pPushConstantRanges = NULL;

    if (vkCreatePipelineLayout(sr_device.l, &layoutInfo, NULL, &p->layout) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Create Pipeline Layout");
        return SR_CREATE_FAIL;
    }




    SR_LOG_DEB("Pipeline Config Created");
    return SR_NO_ERROR;
}




void DestroyPipelineConfig(VulkanPipelineConfig* p) {
    VkDevice d = sr_device.l;
    vkDestroyDescriptorPool(d, p->descrip.descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(d, p->descrip.descriptorLayout, NULL);
    vkDestroyPipelineLayout(d, p->layout, NULL);
    SR_LOG_DEB("Pipeline Config Destroyed");
}
