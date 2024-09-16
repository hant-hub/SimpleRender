#include "memory.h"
#include "command.h"
#include "error.h"
#include "init.h"
#include <string.h>
#include <vulkan/vulkan_core.h>

ErrorCode findMemoryType(uint32_t typefilter, VkPhysicalDevice p, VkMemoryPropertyFlags properties, uint32_t* out) {
    VkPhysicalDeviceMemoryProperties devMemProps = {0};
    vkGetPhysicalDeviceMemoryProperties(p, &devMemProps);

    for (uint32_t i = 0; i < devMemProps.memoryTypeCount; i++) {
        if ((typefilter & (1 << i)) &&
           ((devMemProps.memoryTypes[i].propertyFlags & properties) == properties) ) {
            *out = i;
            return SR_NO_ERROR;
        }
    }

    SR_LOG_ERR("Failed to find suitable Memory");
    return SR_INVALID;
}



ErrorCode CreateBuffer(VulkanDevice* d, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memFlags, 
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




void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VulkanDevice* d, VulkanCommand* c) {
    VkCommandBuffer cmd = beginSingleTimeCommand(d->l, c->pool);

    VkBufferCopy copyRegion = {0};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(cmd, srcBuffer, dstBuffer, 1, &copyRegion); 

    endSingleTimeCommand(cmd, c->pool, d);
}


void DestroyStaticBuffer(VkDevice d, StaticBuffer* b) {
    vkDestroyBuffer(d, b->buf, NULL);
    vkFreeMemory(d, b->mem, NULL);
}

void DestroyDynamicBuffer(VkDevice d, DynamicBuffer* b) {
    vkDestroyBuffer(d, b->buf, NULL);
    vkFreeMemory(d, b->mem, NULL);
}



ErrorCode CreateStaticBuffer(VulkanDevice* d, VulkanCommand* cmd, VkBufferUsageFlags usage, const void* data, u32 size, StaticBuffer* buf) {
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;
    CreateBuffer(d, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 &stagingBuffer, &stagingMemory);

    void* dest;
    vkMapMemory(d->l, stagingMemory, 0, size, 0, &dest);
    memcpy(dest, data, size);
    vkUnmapMemory(d->l, stagingMemory);

    buf->size = size;
    CreateBuffer(d, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 &buf->buf, &buf->mem);

    CopyBuffer(stagingBuffer, buf->buf, size, d, cmd);
    vkDestroyBuffer(d->l, stagingBuffer, NULL);
    vkFreeMemory(d->l, stagingMemory, NULL);
    return SR_NO_ERROR;
}

ErrorCode CreateDynamicBuffer(VulkanDevice* d, VulkanCommand* cmd, u32 size, DynamicBuffer* buf, VkBufferUsageFlags usage) {
    CreateBuffer(d, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 &buf->buf, &buf->mem);
    vkMapMemory(d->l, buf->mem, 0, size, 0, &buf->buffer);
    return SR_NO_ERROR;
}
