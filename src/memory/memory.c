#include "memory.h"
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



ErrorCode CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memFlags, 
                              VkBuffer* buffer, VkDeviceMemory* mem) {

    VkBufferCreateInfo bufferInfo = {0};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkDevice d = sr_device.l;

    if (vkCreateBuffer(d, &bufferInfo, NULL, buffer) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to create Vertex Buffer");
        return SR_CREATE_FAIL;
    }

    VkMemoryRequirements memReq = {0};
    vkGetBufferMemoryRequirements(d, *buffer, &memReq);

    VkPhysicalDeviceMemoryProperties devMemProps = {0};
    vkGetPhysicalDeviceMemoryProperties(sr_device.p, &devMemProps);

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


    if (vkAllocateMemory(d, &allocInfo, NULL, mem) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Allocate Memory");
        return SR_CREATE_FAIL;
    }

    vkBindBufferMemory(d, *buffer, *mem, 0);


    return SR_NO_ERROR;
}




void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VulkanCommand* c) {
    VkDevice d = sr_device.l;
    VkCommandBuffer cmd = beginSingleTimeCommand(c->pool);

    VkBufferCopy copyRegion = {0};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(cmd, srcBuffer, dstBuffer, 1, &copyRegion); 

    endSingleTimeCommand(cmd, c->pool);
}


void DestroyStaticBuffer(StaticBuffer* b) {
    vkDestroyBuffer(sr_device.l, b->vhandle, NULL);
    vkFreeMemory(sr_device.l, b->mem, NULL);
}

void DestroyDynamicBuffer(DynamicBuffer* b) {
    vkDestroyBuffer(sr_device.l, b->vhandle, NULL);
    vkFreeMemory(sr_device.l, b->mem, NULL);
}



ErrorCode CreateStaticBuffer(VkBufferUsageFlags usage, const void* data, u32 size, StaticBuffer* buf) {
    VkDevice d = sr_device.l;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;
    CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 &stagingBuffer, &stagingMemory);

    void* dest;
    vkMapMemory(d, stagingMemory, 0, size, 0, &dest);
    memcpy(dest, data, size);
    vkUnmapMemory(d, stagingMemory);

    buf->size = size;
    CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 &buf->vhandle, &buf->mem);

    CopyBuffer(stagingBuffer, buf->vhandle, size, &sr_context.cmd);
    vkDestroyBuffer(d, stagingBuffer, NULL);
    vkFreeMemory(d, stagingMemory, NULL);
    return SR_NO_ERROR;
}

ErrorCode CreateDynamicBuffer(u32 size, DynamicBuffer* buf, VkBufferUsageFlags usage) {
    CreateBuffer(size, usage,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 &buf->vhandle, &buf->mem);
    vkMapMemory(sr_device.l, buf->mem, 0, size, 0, &buf->buffer);
    buf->size = size;
    return SR_NO_ERROR;
}
