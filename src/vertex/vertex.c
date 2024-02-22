#include "vertex.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include <string.h>
#include <vulkan/vulkan_core.h>



ErrorCode CreateVertexBuffer(VertexBuffer* buffer, const void* data, uint32_t bufSize, VulkanDevice* d) {

    buffer->size = bufSize;
    VkBufferCreateInfo bufferInfo = {0};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufSize;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(d->l, &bufferInfo, NULL, &buffer->buf) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to create Vertex Buffer");
        return SR_CREATE_FAIL;
    }

    VkMemoryRequirements memReq = {0};
    vkGetBufferMemoryRequirements(d->l, buffer->buf, &memReq);

    VkPhysicalDeviceMemoryProperties devMemProps = {0};
    vkGetPhysicalDeviceMemoryProperties(d->p, &devMemProps);

    uint32_t typefilter = memReq.memoryTypeBits;
    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
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


    if (vkAllocateMemory(d->l, &allocInfo, NULL, &buffer->mem) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Allocate Memory");
        return SR_CREATE_FAIL;
    }

    vkBindBufferMemory(d->l, buffer->buf, buffer->mem, 0);

    void* dest;
    vkMapMemory(d->l, buffer->mem, 0, bufSize, 0, &dest);
     memcpy(dest, data, bufSize);
    vkUnmapMemory(d->l, buffer->mem);

    SR_LOG_DEB("Vertex Buffer Created");

    return SR_NO_ERROR;
}


void DestroyBuffer(VkDevice d, VertexBuffer* buffer) {
    vkDestroyBuffer(d, buffer->buf, NULL);
    vkFreeMemory(d, buffer->mem, NULL);
}
