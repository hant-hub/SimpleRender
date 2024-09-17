#ifndef SR_VERTEX_H
#define SR_VERTEX_H
#include "memory.h"
#include "vec2.h"
#include "vec3.h"
#include "error.h"
#include "init.h"
#include "pipeline.h"


typedef struct {
    sm_vec2f pos;
    sm_vec2f uv;
} Vertex;

typedef struct {
    StaticBuffer vertexBuffer;
    StaticBuffer indexBuffer;
    uint32_t indexCount;
} GeometryBuffer;



static const VkVertexInputBindingDescription bindingDescription = {
    0,                              //binding
    sizeof(Vertex),                 //Stride
    VK_VERTEX_INPUT_RATE_VERTEX   //Input Rate
};


static const VkVertexInputAttributeDescription attrDescription[3] = {
    //Position
    {
        0,                          //location
        0,                          //binding
        VK_FORMAT_R32G32_SFLOAT,    //format
        offsetof(Vertex, pos)       //offset
    },
    //UV
    {
        1,                          //location
        0,                          //binding
        VK_FORMAT_R32G32_SFLOAT, //format
        offsetof(Vertex, uv)     //offset
    }
};


ErrorCode CreateStaticGeometry(GeometryBuffer* buffer, const void* verticies, const void* indicies, uint32_t vertSize, uint32_t indSize, VulkanDevice* d, VulkanCommand* c);
void DestroyGeometryBuffer(VkDevice d, GeometryBuffer* buffer);




#endif
