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
} BufferHandle;


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
    u32 bindPoint;
} VulkanDescriptorInfo;

typedef enum {
    SR_DESC_UNIFORM,
    SR_DESC_STORAGE,
    SR_DESC_SAMPLER
} DescriptorType;

typedef struct {
    DescriptorType type;
    VkShaderStageFlagBits stage;
    u32 misc;
} DescriptorDetail;


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


//vertex

typedef struct {
    VkVertexInputRate rate;
    VkFormat format;
    u32 size;
} AttrConfig;

typedef struct {
    VkVertexInputAttributeDescription* attrs;
    VkVertexInputBindingDescription* bindings;
    u32 size;
} VulkanMultiVertexInput;

typedef struct {
    VkVertexInputAttributeDescription* attrs;
    VkVertexInputBindingDescription binding;
    u32 size;
} VulkanVertexInput;

#define VulkanVertToConfig(v) \
    (VulkanConfigInput){v.attrs, &v.binding, v.size, 1}

#define VulkanMultiVertToConfig(v) \
    (VulkanConfigInput){v.attrs, v.bindings, v.size, v.size}


typedef struct {
    VkVertexInputAttributeDescription* attrs;
    VkVertexInputBindingDescription* bindings;
    u32 asize;
    u32 bsize;
} VulkanConfigInput;

ErrorCode MultiCreateVertAttr(VkVertexInputAttributeDescription* attrOut, VkVertexInputBindingDescription* bindOut, AttrConfig* configs, u32 numAttrs);
ErrorCode CreateVertAttr(VkVertexInputAttributeDescription* attrOut, VkVertexInputBindingDescription* bindOut, AttrConfig* configs, u32 numAttrs);

//renderpass

typedef enum {
    COLOR_ONLY = 0,
    DEPTH = 1 << 0
} RenderPassAttachmentFlags;


typedef struct {
    u32 attachmentFlags;
} RenderPassConfig;




ErrorCode CreatePass(RenderPass* r);
void DestroyPass(RenderPass* r);

//pipeline Config
ErrorCode CreatePipelineConfig(VulkanShader* s, VulkanConfigInput v, VulkanPipelineConfig* p);
void DestroyPipelineConfig(VulkanPipelineConfig* p);

//bindings
ErrorCode CreateDescriptorSetConfig(VulkanPipelineConfig* config, DescriptorDetail* layout, u32 size);
ErrorCode SetImage(VkImageView v, VkSampler s, VulkanPipelineConfig* config, u32 index, u32 arrayIndex);
ErrorCode SetImages(VkImageView *v, VkSampler *s, VulkanPipelineConfig* config, u32 index, u32 size);
ErrorCode SetBuffer(VulkanPipelineConfig* config, VkBufferUsageFlags usage, BufferHandle* handles, u32 index);



#endif