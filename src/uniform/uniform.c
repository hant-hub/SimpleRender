#include "uniform.h"


ErrorCode CreateUniformBuffer(UniformHandles* handles, VulkanPipelineConfig* config, VulkanDevice* d) {
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


        VkWriteDescriptorSet write = {0};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = config->descriptorSet[i];
        write.dstBinding = 0;
        write.dstArrayElement = 0;

        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.descriptorCount = 1;

        write.pBufferInfo = &bufInfo;
        write.pImageInfo = NULL;
        write.pTexelBufferView = NULL;

        vkUpdateDescriptorSets(d->l, 1, &write, 0, NULL);
    }


    return SR_NO_ERROR;
}

void DestroyUniformBuffer(VkDevice d, UniformHandles* handles) {
    for (size_t i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(d, handles->bufs[i], NULL);
        vkFreeMemory(d, handles->mem[i], NULL);
    }
}
