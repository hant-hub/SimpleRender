#ifndef SR_VERTEX_H
#define SR_VERTEX_H
#include "vec2.h"
#include "vec3.h"
#include "error.h"
#include "init.h"
#include <vulkan/vulkan_core.h>


typedef struct {
    vec2float pos;
    vec3float color;
} Vertex;

typedef struct {
    VkBuffer buf;
    uint32_t size;
    VkDeviceMemory mem;
} VertexBuffer;

static const VkVertexInputBindingDescription bindingDescription = {
    0,                              //binding
    sizeof(Vertex),                 //Stride
    VK_VERTEX_INPUT_RATE_VERTEX     //Input Rate
};

static const VkVertexInputAttributeDescription attrDescription[2] = {
    //Position
    {
        0,                          //location
        0,                          //binding
        VK_FORMAT_R32G32_SFLOAT,    //format
        offsetof(Vertex, pos)       //offset
    },
    //Color
    {
        1,                          //location
        0,                          //binding
        VK_FORMAT_R32G32B32_SFLOAT, //format
        offsetof(Vertex, color)     //offset
    }
};


ErrorCode CreateVertexBuffer(VertexBuffer* buffer, const void* data, uint32_t bufSize, VulkanDevice* d);
void DestroyBuffer(VkDevice d, VertexBuffer* buffer);

#endif
