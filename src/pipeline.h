#ifndef SR_PIPE_H
#define SR_PIPE_H

#include "error.h"
#include "init.h"
#include "log.h"
#include "error.h"
#include "util.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

typedef struct {
    VkShaderModule vertex;
    VkShaderModule fragment;
} VulkanShader;

typedef struct {
    VkRenderPass pass;
    VkDescriptorSetLayout descriptors;
    VkPipelineLayout layout;
    VkDynamicState states[2];
    VkPipelineDynamicStateCreateInfo dynamic;
    VkPipelineVertexInputStateCreateInfo vertInput;
    VkPipelineInputAssemblyStateCreateInfo assembly;
    VkPipelineViewportStateCreateInfo view;
    VkPipelineRasterizationStateCreateInfo raster;
    VkPipelineMultisampleStateCreateInfo multisample;
    VkPipelineColorBlendStateCreateInfo colorState;
    VkPipelineColorBlendAttachmentState colorattachment;
    VkPipelineShaderStageCreateInfo stages[2];
} VulkanPipelineConfig;

typedef struct {
    VkViewport view;
    VkRect2D scissor;
    VkPipeline pipeline;
    VkRenderPass pass;

} VulkanPipeline;

void DestroyShaderProg(VkDevice d, VulkanShader* s);
void DestroyPipelineConfig(VkDevice d, VulkanPipelineConfig* p);
void DestroyPipeline(VkDevice d, VulkanPipeline* p);

ErrorCode CreateShaderProg(VkDevice d, const char* vertex, const char* frag, VulkanShader* s);
ErrorCode CreatePipelineConfig(VulkanDevice* d, VulkanContext* c, VkFormat swapFormat, VulkanShader* s, VulkanPipelineConfig* p);
ErrorCode CreatePipeline(VulkanDevice* d, VulkanContext* c, VulkanShader* s, VulkanPipelineConfig* con, VulkanPipeline* p);





#endif
