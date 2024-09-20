#include "config.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include "memory.h"
#include <vulkan/vulkan_core.h>


#define NUM_SUPPORTED_TYPES 3

ErrorCode CreateDescriptorSetConfig(VulkanPipelineConfig* config, DescriptorType* layout, DescriptorDetail* access, u32 size) {

    VkDescriptorSetLayoutBinding bindings[size]; 
    VkDescriptorPoolSize poolSizes[NUM_SUPPORTED_TYPES];
    u32 typeNums[NUM_SUPPORTED_TYPES] = {0};

    for (u32 i = 0; i < size; i++) {
        switch (layout[i]) {
            case SR_DESC_UNIFORM: {
               bindings[i] = (VkDescriptorSetLayoutBinding){
                    .binding = i,
                    .stageFlags = access[i].stage,
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
                    .stageFlags = access[i].stage,
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
                    .stageFlags = access[i].stage,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .pImmutableSamplers = NULL,
                    .descriptorCount = access[i].misc,
               };
               typeNums[2] += access[i].misc;
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

ErrorCode SetImage(VkImageView v, VkSampler s, VulkanPipelineConfig* config, u32 index, u32 arrayIndex) {

    for (u32 i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorImageInfo imgInfo = {
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = v,
            .sampler = s 
        };

        VkWriteDescriptorSet write = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = config->descrip.descriptorSet[i],
            .dstBinding = index,
            .dstArrayElement = arrayIndex,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .pBufferInfo = NULL,
            .pImageInfo = &imgInfo,
            .pTexelBufferView = NULL
        };

        vkUpdateDescriptorSets(sr_device.l, 1, &write, 0, NULL);
    }
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



ErrorCode SetBuffer(VulkanPipelineConfig* config, BufferHandle* handles, u32 index) {

    VkDeviceSize bufSize = sizeof(UniformObj) * SR_MAX_INSTANCES;
    VkDevice d = sr_device.l;

    for (size_t i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        CreateBuffer(bufSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     &handles->bufs[i], &handles->mem[i]);

        vkMapMemory(d, handles->mem[i], 0, bufSize, 0, &handles->objs[i]);
    }

    for (size_t i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufInfo = {0};
        bufInfo.buffer = handles->bufs[i];
        bufInfo.offset = 0;
        bufInfo.range = bufSize;


        VkWriteDescriptorSet write = {0};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = config->descrip.descriptorSet[i];
        write.dstBinding = 0;
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.descriptorCount = 1;
        write.pBufferInfo = &bufInfo;
        write.pImageInfo = NULL;
        write.pTexelBufferView = NULL;

        vkUpdateDescriptorSets(d, 1, &write, 0, NULL);
    }



    return SR_NO_ERROR;
}

