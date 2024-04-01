#include "uniform.h"
#include <vulkan/vulkan_core.h>


ErrorCode CreateUniformBuffer(UniformHandles* handles, Texture* t, VulkanPipelineConfig* config, VulkanDevice* d) {
    VkDeviceSize bufSize = sizeof(UniformObj);

    for (size_t i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        CreateBuffer(d, bufSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     &handles->bufs[i], &handles->mem[i]);

        vkMapMemory(d->l, handles->mem[i], 0, bufSize, 0, &handles->objs[i]);
    }

    for (size_t i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufInfo = {0};
        bufInfo.buffer = handles->bufs[i];
        bufInfo.offset = 0;
        bufInfo.range = bufSize;

        VkDescriptorImageInfo imgInfo = {0};
        imgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imgInfo.imageView = t->view;
        imgInfo.sampler = t->sampler;

        VkWriteDescriptorSet write[2] = {0};
        write[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write[0].dstSet = config->descriptorSet[i];
        write[0].dstBinding = 0;
        write[0].dstArrayElement = 0;
        write[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write[0].descriptorCount = 1;
        write[0].pBufferInfo = &bufInfo;
        write[0].pImageInfo = NULL;
        write[0].pTexelBufferView = NULL;

        write[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write[1].dstSet = config->descriptorSet[i];
        write[1].dstBinding = 1;
        write[1].dstArrayElement = 0;
        write[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write[1].descriptorCount = 1;
        write[1].pBufferInfo = NULL;
        write[1].pImageInfo = &imgInfo;
        write[1].pTexelBufferView = NULL;

        vkUpdateDescriptorSets(d->l, 2, write, 0, NULL);
    }


    return SR_NO_ERROR;
}

void DestroyUniformBuffer(VkDevice d, UniformHandles* handles) {
    for (size_t i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(d, handles->bufs[i], NULL);
        vkFreeMemory(d, handles->mem[i], NULL);
    }
}
