#ifndef SR_MEMORY_H
#define SR_MEMORY_H

#include "error.h"
#include "init.h"

typedef struct VulkanCommand VulkanCommand;

typedef struct {
    uint32_t size;
    VkBuffer buf;
    VkDeviceMemory mem;
} Buffer;


ErrorCode findMemoryType(uint32_t typefilter, VkPhysicalDevice p, VkMemoryPropertyFlags properties, uint32_t* out);
ErrorCode CreateBuffer(VulkanDevice* d, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memFlags, VkBuffer* buffer, VkDeviceMemory* mem);
void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VulkanDevice* d, VulkanCommand* c);

void DestroyBuffer(VkDevice d, Buffer* b);



#endif
