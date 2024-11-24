#include "config.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include "memory.h"
#include <vulkan/vulkan_core.h>


#define NUM_SUPPORTED_TYPES 3

ErrorCode CreateDescriptorSetConfig(VulkanPipelineConfig* config, DescriptorDetail* layout, u32 size) {

    VkDescriptorSetLayoutBinding bindings[size]; 
    VkDescriptorPoolSize poolSizes[NUM_SUPPORTED_TYPES];
    u32 typeNums[NUM_SUPPORTED_TYPES] = {0};

    for (u32 i = 0; i < size; i++) {
        switch (layout[i].type) {
            case SR_DESC_UNIFORM: {
               bindings[i] = (VkDescriptorSetLayoutBinding){
                    .binding = i,
                    .stageFlags = layout[i].stage,
                    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .pImmutableSamplers = NULL,
                    .descriptorCount = 1,
               };
               typeNums[0]++;
               SR_LOG_DEB("\tBinding %d bound to Buffer", i);

               break;
            }
            case SR_DESC_STORAGE: {
               bindings[i] = (VkDescriptorSetLayoutBinding){
                    .binding = i,
                    .stageFlags = layout[i].stage,
                    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    .pImmutableSamplers = NULL,
                    .descriptorCount = 1,
               };
               typeNums[1]++;
               SR_LOG_DEB("\tBinding %d bound to Buffer", i);
                break;
            }
            case SR_DESC_SAMPLER: {
               bindings[i] = (VkDescriptorSetLayoutBinding){
                    .binding = i,
                    .stageFlags = layout[i].stage,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .pImmutableSamplers = NULL,
                    .descriptorCount = layout[i].misc,
               };
               typeNums[2] += layout[i].misc;
               SR_LOG_DEB("\tBinding %d bound to Image", i);
               break;
            }
            default: {
                SR_LOG_WAR("Invalid Descriptor Types!");
                break;
            }
        }
    }

    VkDevice d = sr_device.l;

    VkDescriptorSetLayoutCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pBindings = bindings,
        .bindingCount = size
    };

    if (vkCreateDescriptorSetLayout(d, &createInfo, NULL, &config->descrip.descriptorLayout) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Create Descriptor Layout");
        return SR_CREATE_FAIL;
    }

    u32 maxsize = 0;
    for (u32 i = 0; i < NUM_SUPPORTED_TYPES; i++) maxsize = maxsize < typeNums[i] ? typeNums[i] : maxsize;
    
    int poolSizeTop = 0;
    if (typeNums[0])
        poolSizes[poolSizeTop++] = (VkDescriptorPoolSize) { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         typeNums[0] * (u32)SR_MAX_FRAMES_IN_FLIGHT};
    if (typeNums[1])
        poolSizes[poolSizeTop++] = (VkDescriptorPoolSize) { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         typeNums[1] * (u32)SR_MAX_FRAMES_IN_FLIGHT};
    if (typeNums[2])
        poolSizes[poolSizeTop++] = (VkDescriptorPoolSize) { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, typeNums[2] * (u32)SR_MAX_FRAMES_IN_FLIGHT};
    
    VkDescriptorPoolCreateInfo poolInfo = (VkDescriptorPoolCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = (u32)SR_MAX_FRAMES_IN_FLIGHT,
        .pPoolSizes = poolSizes,
        .poolSizeCount = poolSizeTop
   };


    if (vkCreateDescriptorPool(d, &poolInfo, NULL, &config->descrip.descriptorPool) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Create Descriptor Pool");
        return SR_CREATE_FAIL;
    }

    VkDescriptorSetLayout layouts[SR_MAX_FRAMES_IN_FLIGHT];
    for (u32 i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++)
        layouts[i] = config->descrip.descriptorLayout;

    VkDescriptorSetAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = config->descrip.descriptorPool,
        .descriptorSetCount = (u32) SR_MAX_FRAMES_IN_FLIGHT,
        .pSetLayouts = layouts
    };

    if (vkAllocateDescriptorSets(d, &allocInfo, config->descrip.descriptorSet) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Create Descriptor Set");
        return SR_CREATE_FAIL;
    }

    return SR_NO_ERROR;
}

ErrorCode SetImage(VkImageView v, VkSampler s, VulkanPipelineConfig* config, u32 index, u32 arrayIndex, u32 set) {

    VkDescriptorImageInfo imgInfo = {
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .imageView = v,
        .sampler = s 
    };

    VkWriteDescriptorSet write = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = config->descrip.descriptorSet[set],
        .dstBinding = index,
        .dstArrayElement = arrayIndex,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .pBufferInfo = NULL,
        .pImageInfo = &imgInfo,
        .pTexelBufferView = NULL
    };

    vkUpdateDescriptorSets(sr_device.l, 1, &write, 0, NULL);
    return SR_NO_ERROR;
}

ErrorCode SetImages(VkImageView *v, VkSampler *s, VulkanPipelineConfig* config, u32 index, u32 size) {

    for (u32 i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        
        VkDescriptorImageInfo imgInfo[size];
        for (u32 j = 0; j < size; j++) {
            imgInfo[j] = (VkDescriptorImageInfo){
                        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        .imageView = v[j],
                        .sampler = s[j] 
                    };
        }

        VkWriteDescriptorSet write = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = config->descrip.descriptorSet[i],
            .dstBinding = index,
            .dstArrayElement = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = size,
            .pBufferInfo = NULL,
            .pImageInfo = imgInfo,
            .pTexelBufferView = NULL
        };

        vkUpdateDescriptorSets(sr_device.l, 1, &write, 0, NULL);
    }
    return SR_NO_ERROR;
}



ErrorCode SetBuffers(VulkanPipelineConfig* config, DescriptorType usage, Buffer* handles, size_t num, u32 index) {
    VkDevice d = sr_device.l;
    VkDescriptorType type;

    switch (usage) {
        case SR_DESC_STORAGE: {
            type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            break;
        }
        case SR_DESC_UNIFORM: {
            type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            break;
        }
        default: {
            SR_LOG_ERR("Invalid Descriptor Type");
            return SR_INVALID;
        }
    }

    
    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufInfo = {0};
        bufInfo.buffer = handles[i].vhandle;
        bufInfo.offset = 0;
        bufInfo.range = handles[i].size;

        VkWriteDescriptorSet write = {0};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = config->descrip.descriptorSet[i];
        write.dstBinding = index;
        write.dstArrayElement = 0;
        write.descriptorType = type;
        write.descriptorCount = 1;
        write.pBufferInfo = &bufInfo;
        write.pImageInfo = NULL;
        write.pTexelBufferView = NULL;

        vkUpdateDescriptorSets(d, 1, &write, 0, NULL);
    }


    return SR_NO_ERROR;
}

ErrorCode SetBuffer(VulkanPipelineConfig* config, DescriptorType usage, Buffer* handle, u32 index, u32 set) {
    VkDeviceSize bufSize = handle->size;
    VkDevice d = sr_device.l;
    VkBufferUsageFlags bufusage;
    VkDescriptorType type;

    switch (usage) {
        case SR_DESC_STORAGE: {
            bufusage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            break;
        }
        case SR_DESC_UNIFORM: {
            bufusage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            break;
        }
        default: {
            SR_LOG_ERR("Invalid Descriptor Type");
            return SR_INVALID;
        }
    }

    VkDescriptorBufferInfo bufInfo = {0};
    bufInfo.buffer = handle->vhandle;
    bufInfo.offset = 0;
    bufInfo.range = bufSize;
    

    VkWriteDescriptorSet write = {0};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = config->descrip.descriptorSet[set];
    write.dstBinding = index;
    write.dstArrayElement = 0;
    write.descriptorType = type;
    write.descriptorCount = 1;
    write.pBufferInfo = &bufInfo;
    write.pImageInfo = NULL;
    write.pTexelBufferView = NULL;

    vkUpdateDescriptorSets(d, 1, &write, 0, NULL);


    return SR_NO_ERROR;
}

