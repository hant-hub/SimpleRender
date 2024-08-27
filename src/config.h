#ifndef SR_CONFIG_H
#define SR_CONFIG_H

#include "common.h"
#include "error.h"
#include "init.h"
#include "mat4.h"
#include <vulkan/vulkan_core.h>

typedef struct {
    sm_mat4f model;
    sm_mat4f view;
    sm_mat4f proj;
} UniformObj;

typedef struct {
    VkBuffer bufs[SR_MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory mem[SR_MAX_FRAMES_IN_FLIGHT];
    void* objs[SR_MAX_FRAMES_IN_FLIGHT];
} UniformHandles;


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

typedef enum {
    SR_DESC_BUF,
    SR_DESC_SAMPLER
} DescriptorType;



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




//renderpass
ErrorCode CreatePass(RenderPass* r, VulkanDevice* d, VulkanContext* c);
void DestroyPass(VkDevice d, RenderPass* r);

//pipeline Config
ErrorCode CreatePipelineConfig(VulkanDevice* d, VulkanContext* c, VulkanShader* s, VulkanPipelineConfig* p);
void DestroyPipelineConfig(VkDevice d, VulkanPipelineConfig* p);

//bindings
ErrorCode CreateDescriptorSetConfig(VulkanDevice* d, VulkanPipelineConfig* config, DescriptorType* layout, VkShaderStageFlagBits* access, u32 size);
ErrorCode SetImage(VulkanDevice* d, VkImageView v, VkSampler s, VulkanPipelineConfig* config, u32 index);
ErrorCode SetBuffer(VulkanDevice* d, VulkanPipelineConfig* config, UniformHandles* handles, u32 index);


#endif
