#include "vertex.h"
#include "command.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include "pipeline.h"
#include "util.h"
#include <stddef.h>
#include <string.h>
#include <vulkan/vulkan_core.h>


static ErrorCode CreateBuffer(VulkanDevice* d, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memFlags, 
                              VkBuffer* buffer, VkDeviceMemory* mem) {

    VkBufferCreateInfo bufferInfo = {0};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(d->l, &bufferInfo, NULL, buffer) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to create Vertex Buffer");
        return SR_CREATE_FAIL;
    }

    VkMemoryRequirements memReq = {0};
    vkGetBufferMemoryRequirements(d->l, *buffer, &memReq);

    VkPhysicalDeviceMemoryProperties devMemProps = {0};
    vkGetPhysicalDeviceMemoryProperties(d->p, &devMemProps);

    uint32_t typefilter = memReq.memoryTypeBits;
    VkMemoryPropertyFlags properties = memFlags;
    uint32_t selected;
    bool valid = FALSE;



    for (uint32_t i = 0; i < devMemProps.memoryTypeCount; i++) {
        if ((typefilter & (1 << i)) &&
           ((devMemProps.memoryTypes[i].propertyFlags & properties) == properties) ) {
            selected = i;
            valid = TRUE;
        }
    }

    if (valid == FALSE){ 
        SR_LOG_ERR("Failed to find suitable Memory");
        return SR_INVALID;
    }

    VkMemoryAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = selected;


    if (vkAllocateMemory(d->l, &allocInfo, NULL, mem) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Allocate Memory");
        return SR_CREATE_FAIL;
    }

    vkBindBufferMemory(d->l, *buffer, *mem, 0);


    return SR_NO_ERROR;
}

static void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VulkanDevice* d, VulkanCommand* c) {
    VkCommandBufferAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = c->pool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer cmd;
    vkAllocateCommandBuffers(d->l, &allocInfo, &cmd);

    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd, &beginInfo);

    VkBufferCopy copyRegion = {0};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(cmd, srcBuffer, dstBuffer, 1, &copyRegion); 

    vkEndCommandBuffer(cmd);

    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    vkQueueSubmit(d->graph, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(d->graph);

    vkFreeCommandBuffers(d->l, c->pool, 1, &cmd);
}


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


ErrorCode CreateStaticGeometry(GeometryBuffer* buffer, const void* verticies, const void* indicies, uint32_t vertSize, uint32_t indSize, VulkanDevice* d, VulkanCommand* c) {
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;
    CreateBuffer(d, vertSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 &stagingBuffer, &stagingMemory);

    void* dest;
    vkMapMemory(d->l, stagingMemory, 0, vertSize, 0, &dest);
    memcpy(dest, verticies, vertSize);
    vkUnmapMemory(d->l, stagingMemory);

    buffer->vertexBuffer.size = vertSize;
    CreateBuffer(d, vertSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 &buffer->vertexBuffer.buf, &buffer->vertexBuffer.mem);

    CopyBuffer(stagingBuffer, buffer->vertexBuffer.buf, vertSize, d, c);
    vkDestroyBuffer(d->l, stagingBuffer, NULL);
    vkFreeMemory(d->l, stagingMemory, NULL);

    //index buffer
    CreateBuffer(d, indSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 &stagingBuffer, &stagingMemory);

    vkMapMemory(d->l, stagingMemory, 0, indSize, 0, &dest);
    memcpy(dest, indicies, indSize);
    vkUnmapMemory(d->l, stagingMemory);

    buffer->indexBuffer.size = indSize;
    buffer->indexCount = indSize / sizeof(uint16_t);
    CreateBuffer(d, indSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 &buffer->indexBuffer.buf, &buffer->indexBuffer.mem);

    CopyBuffer(stagingBuffer, buffer->indexBuffer.buf, indSize, d, c);
    vkDestroyBuffer(d->l, stagingBuffer, NULL);
    vkFreeMemory(d->l, stagingMemory, NULL);

    return SR_NO_ERROR;
}


void DestroyBuffer(VkDevice d, GeometryBuffer* buffer) {
    vkDestroyBuffer(d, buffer->vertexBuffer.buf, NULL);
    vkFreeMemory(d, buffer->vertexBuffer.mem, NULL);

    vkDestroyBuffer(d, buffer->indexBuffer.buf, NULL);
    vkFreeMemory(d, buffer->indexBuffer.mem, NULL);
}
