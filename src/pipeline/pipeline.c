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







ErrorCode CreatePipeline(VulkanDevice* d, VulkanShader* s, VulkanPipelineConfig* con, VulkanPipeline* p, RenderPass* r) {
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
    pipelineInfo.renderPass = r->pass;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;


    p->pass = r;

    if (vkCreateGraphicsPipelines(d->l, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &p->pipeline) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Create Graphics Pipeline");
        return SR_CREATE_FAIL;
    }

    SR_LOG_DEB("Graphics Pipeline Created");
    return SR_NO_ERROR;
}





void DestroyShaderProg(VkDevice d, VulkanShader* s) {
    vkDestroyShaderModule(d, s->vertex, NULL);
    vkDestroyShaderModule(d, s->fragment, NULL);
}


void DestroyPipeline(VkDevice d, VulkanPipeline* p) {
    vkDestroyPipeline(d, p->pipeline, NULL);
    SR_LOG_DEB("Graphics Pipeline Destroyed");
}

