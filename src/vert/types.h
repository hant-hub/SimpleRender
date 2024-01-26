#ifndef VERTEX_TYPES_H
#define VERTEX_TYPES_H
#include "../Vulkan.h"
#include "../lib/SimpleMath/src/include/linalg.h"
#include "../state.h"
#include <error.h>
#include <vulkan/vulkan_core.h>

typedef struct Vertex {
    vec2float position;
    vec3float color;
} Vertex;

typedef struct VertexBuffer {
    VkBuffer buffer;
    VkDeviceMemory memory;
} VertexBuffer;

typedef struct IndexBuffer {
    VkBuffer buffer;
    VkDeviceMemory memory;
} IndexBuffer;

typedef struct VertexAttrDescription {
    VkVertexInputAttributeDescription val[2];
} VertexAttrDescription;

static const Vertex verticies[] = {
    {{-0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
    {{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}
};

static const uint16_t indicies[] = {
    0, 1, 2, 2, 3, 0
};


void DestroyBuffer(VkDevice d, VkBuffer* b, VkDeviceMemory* mem);
ErrorCode CreateVertexBuffer(VulkanDevice* d, Command* c, VertexBuffer* vertexBuffer);
ErrorCode CreateIndexBuffer(VulkanDevice* d, Command* c, IndexBuffer* vertexBuffer);
void copyBuffer(VulkanDevice* d, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool pool);

VkVertexInputBindingDescription GetVertexBindingDescription();
VertexAttrDescription GetVertexAttributeDescriptions();



#endif
