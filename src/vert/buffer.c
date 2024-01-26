#include "types.h"
#include <error.h>
#include <vulkan/vulkan_core.h>


static uint32_t findMemoryType(VkPhysicalDevice d, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties props;
    vkGetPhysicalDeviceMemoryProperties(d, &props);
    
    for (uint32_t i = 0; i < props.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && 
           ((props.memoryTypes[i].propertyFlags & properties) == properties)) {
            return i;
        }
    }

    return 0;
}


void DestroyBuffer(VkDevice d, VkBuffer* b, VkDeviceMemory* mem) {
    vkDestroyBuffer(d, *b, NULL);
    vkFreeMemory(d, *mem, NULL);
}

ErrorCode CreateBuffer(VulkanDevice* d, VkDeviceSize size, VkBufferUsageFlags usage,
                        VkMemoryPropertyFlags properties, VkBuffer* vb, VkDeviceMemory* mem) {

    VkBufferCreateInfo bufferInfo = {0}; 
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;

    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(d->logical.device, &bufferInfo, NULL, vb) != VK_SUCCESS) {
        fprintf(stderr, ERR_COLOR("Vertex Buffer Creation Failed"));
        return Error;
    }

    VkMemoryRequirements memreq;
    vkGetBufferMemoryRequirements(d->logical.device, *vb, &memreq);

    VkMemoryAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memreq.size;
    allocInfo.memoryTypeIndex = findMemoryType(d->physical.device,
            memreq.memoryTypeBits,
            properties);

    if (vkAllocateMemory(d->logical.device, &allocInfo, NULL, mem) != VK_SUCCESS) {
        fprintf(stderr, ERR_COLOR("Vertex Buffer Memory Allocation Failed"));
        return Error;
    }

    vkBindBufferMemory(d->logical.device, *vb, *mem, 0);

    return NoError;
}

void copyBuffer(VulkanDevice* d, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool pool) {
    VkCommandBufferAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = pool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandbuffer;
    vkAllocateCommandBuffers(d->logical.device, &allocInfo, &commandbuffer);


    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandbuffer, &beginInfo);
    VkBufferCopy copyRegion = {0};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandbuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    vkEndCommandBuffer(commandbuffer);


    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandbuffer;

    vkQueueSubmit(d->logical.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(d->logical.graphicsQueue);

    vkFreeCommandBuffers(d->logical.device, pool, 1, &commandbuffer);

}

ErrorCode CreateVertexBuffer(VulkanDevice* d, Command* c, VertexBuffer *vertexBuffer) {
    VkDeviceSize buffersize = sizeof(verticies);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    ErrorCode result = CreateBuffer(d, buffersize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer,
                 &stagingBufferMemory);
    if (result == Error)
        return Error;

    void* data;
    vkMapMemory(d->logical.device, stagingBufferMemory, 0, buffersize, 0, &data);
        memcpy(data, verticies, (size_t)buffersize);
    vkUnmapMemory(d->logical.device, stagingBufferMemory);

    result = CreateBuffer(d, buffersize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertexBuffer->buffer,
                 &vertexBuffer->memory);
    if (result == Error)
        return Error;

    copyBuffer(d, stagingBuffer, vertexBuffer->buffer, buffersize, c->pool);

    vkDestroyBuffer(d->logical.device, stagingBuffer, NULL);
    vkFreeMemory(d->logical.device, stagingBufferMemory, NULL);

    return NoError;
}

ErrorCode CreateIndexBuffer(VulkanDevice* d, Command* c, IndexBuffer* vertexBuffer) {
    VkDeviceSize buffersize = sizeof(indicies);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    ErrorCode result = CreateBuffer(d, buffersize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer,
                 &stagingBufferMemory);
    if (result == Error)
        return Error;

    void* data;
    vkMapMemory(d->logical.device, stagingBufferMemory, 0, buffersize, 0, &data);
        memcpy(data, indicies, (size_t)buffersize);
    vkUnmapMemory(d->logical.device, stagingBufferMemory);

    result = CreateBuffer(d, buffersize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertexBuffer->buffer,
                 &vertexBuffer->memory);
    if (result == Error)
        return Error;

    copyBuffer(d, stagingBuffer, vertexBuffer->buffer, buffersize, c->pool);

    vkDestroyBuffer(d->logical.device, stagingBuffer, NULL);
    vkFreeMemory(d->logical.device, stagingBufferMemory, NULL);

    return NoError;
    return NoError;
}


