#ifndef SR_MEMORY_H
#define SR_MEMORY_H

#include "common.h"
#include "error.h"
#include "init.h"

typedef struct {
    uint32_t size;
    VkDeviceMemory mem; 
    VkBuffer vhandle;
} Buffer;

typedef struct {
    uint32_t size;
    VkDeviceMemory mem; 
    VkBuffer vhandle;
} StaticBuffer;

typedef struct {
    uint32_t size;
    VkDeviceMemory mem; 
    VkBuffer vhandle;
    void* buffer;
} DynamicBuffer;

ErrorCode findMemoryType(uint32_t typefilter, VkPhysicalDevice p, VkMemoryPropertyFlags properties, uint32_t* out);
ErrorCode CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memFlags, VkBuffer* buffer, VkDeviceMemory* mem);
void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VulkanCommand* c);

void DestroyStaticBuffer(StaticBuffer* b);
void DestroyDynamicBuffer(DynamicBuffer* b);


ErrorCode CreateStaticBuffer(VkBufferUsageFlags usage, const void* data, u32 size, StaticBuffer* buf);
ErrorCode CreateDynamicBuffer(u32 size, DynamicBuffer* buf, VkBufferUsageFlags usage);

#endif
