#ifndef SR_CONFIG_H
#define SR_CONFIG_H

#include "common.h"
#include "error.h"
#include "init.h"
#include <vulkan/vulkan_core.h>


typedef struct {
    VkRenderPass pass;
} RenderPass;

typedef struct {
    VkShaderModule vertex;
    VkShaderModule fragment;
} VulkanShader;

typedef struct {
    VkDescriptorSetLayout descriptorLayout;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet[SR_MAX_FRAMES_IN_FLIGHT];
} VulkanDescriptorInfo;

typedef struct {
    VulkanDescriptorInfo descrip; 

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





ErrorCode CreatePass(RenderPass* r, VulkanDevice* d, VulkanContext* c);
ErrorCode CreatePipelineConfig(VulkanDevice* d, VulkanContext* c, VulkanShader* s, VulkanPipelineConfig* p);


void DestroyPass(VkDevice d, RenderPass* r);
void DestroyPipelineConfig(VkDevice d, VulkanPipelineConfig* p);





#endif
