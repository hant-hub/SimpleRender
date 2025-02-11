#ifndef SR_CONFIG_H
#define SR_CONFIG_H

#include "common.h"
#include "error.h"
#include "init.h"
#include "memory.h"
#include <vulkan/vulkan_core.h>
#include "texture.h"
#include "util.h"



typedef enum {
    SR_ATTATCHMENT_DEPTH
} AttachmentType;

typedef struct {
    AttachmentType type;
    VkFormat format;
    Image image;
} Attachment;

typedef struct {
    u32 firstAttachment;
    u32 numAttachments;
    i32 colorAttachment;
    i32 depthAttachment;
} SubPass;

typedef struct {
    VkRenderPass pass;
    Attachment* configs;
    u32 numAttachments;
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
    VkPipelineDepthStencilStateCreateInfo depth;
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
ErrorCode CreateMultiPass(RenderPass* r, SubPass* passes, u32 numPasses, Attachment* configs, u32 numAttachments);
ErrorCode CreatePass(RenderPass* r, Attachment* configs, u32 numAttachments);
void DestroyPass(RenderPass* r);

//pipeline Config
ErrorCode CreatePipelineConfig(VulkanShader* s, VulkanConfigInput v, VulkanPipelineConfig* p, bool depthEnable);
void DestroyPipelineConfig(VulkanPipelineConfig* p);

//bindings
ErrorCode CreateDescriptorSetConfig(VulkanPipelineConfig* config, DescriptorDetail* layout, u32 size);
ErrorCode SetImage(VkImageView v, VkSampler s, VulkanPipelineConfig* config, u32 index, u32 arrayIndex, u32 set);
ErrorCode SetImages(VkImageView *v, VkSampler *s, VulkanPipelineConfig* config, u32 index, u32 size);
ErrorCode SetBuffer(VulkanPipelineConfig* config, DescriptorType usage, Buffer* handles, u32 index, u32 set);
ErrorCode SetBuffers(VulkanPipelineConfig* config, DescriptorType usage, Buffer* handles, size_t num, u32 index);

#endif
