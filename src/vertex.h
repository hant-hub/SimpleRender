#ifndef SR_VERTEX_H
#define SR_VERTEX_H
#include "vec2.h"
#include "vec3.h"
#include "mat4x4.h"
#include "error.h"
#include "init.h"
#include "pipeline.h"
#include <vulkan/vulkan_core.h>

typedef struct VulkanCommand VulkanCommand;

typedef struct {
    vec2float pos;
    vec3float color;
} Vertex;

typedef struct {
    VkBuffer buf;
    uint32_t size;
    VkDeviceMemory mem;
} Buffer;

typedef struct {
    Buffer vertexBuffer;
    Buffer indexBuffer;
    uint32_t indexCount;
} GeometryBuffer;


typedef struct {
    float model[16];
    float view[16];
    float proj[16];
} UniformObj;

typedef struct {
    VkBuffer bufs[SR_MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory mem[SR_MAX_FRAMES_IN_FLIGHT];
    void* objs[SR_MAX_FRAMES_IN_FLIGHT];
} UniformHandles;

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


ErrorCode CreateStaticGeometry(GeometryBuffer* buffer, const void* verticies, const void* indicies, uint32_t vertSize, uint32_t indSize, VulkanDevice* d, VulkanCommand* c);
void DestroyBuffer(VkDevice d, GeometryBuffer* buffer);
ErrorCode CreateUniformBuffer(UniformHandles* handles, VulkanPipelineConfig* config, VulkanDevice* d);
void DestroyUniformBuffer(VkDevice d, UniformHandles* handles);




#endif
