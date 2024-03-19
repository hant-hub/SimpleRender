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


ErrorCode CreateStaticGeometry(GeometryBuffer* buffer, const void* verticies, const void* indicies, uint32_t vertSize, uint32_t indSize, VulkanDevice* d, VulkanCommand* c) {
    //vertex buffer
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


void DestroyGeometryBuffer(VkDevice d, GeometryBuffer* buffer) {
    DestroyBuffer(d, &buffer->vertexBuffer);
    DestroyBuffer(d, &buffer->indexBuffer);
}
