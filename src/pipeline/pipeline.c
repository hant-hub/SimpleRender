#include "../pipeline.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include "util.h"
#include <stdio.h>
#include <inttypes.h>
#include <vulkan/vulkan_core.h>

ErrorCode CreatePipeline(VulkanDevice* d, VulkanContext* c, VulkanShader* s, VulkanPipeline* p) {
    //Shader stage creation
    VkPipelineShaderStageCreateInfo vertStage = {0};
    vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStage.module = s->vertex;
    vertStage.pName = "main";

    VkPipelineShaderStageCreateInfo fragStage = {0};
    fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStage.module = s->fragment;
    fragStage.pName = "main";

    VkPipelineShaderStageCreateInfo stageInfos[] = {vertStage, fragStage};

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicInfo = {0};
    dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicInfo.dynamicStateCount = ARRAY_SIZE(dynamicStates);
    dynamicInfo.pDynamicStates = dynamicStates;


    VkPipelineVertexInputStateCreateInfo vertInputInfo = {0};
    vertInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertInputInfo.vertexBindingDescriptionCount = 0;
    vertInputInfo.pVertexBindingDescriptions = NULL;
    vertInputInfo.vertexAttributeDescriptionCount = 0;
    vertInputInfo.pVertexAttributeDescriptions = NULL;


    VkPipelineInputAssemblyStateCreateInfo assemInfo = {0};
    assemInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assemInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    assemInfo.primitiveRestartEnable = VK_FALSE;
    

    return SR_NO_ERROR;
}
