#ifndef SR_SPRITE_H
#define SR_SPRITE_H


#include "common.h"
#include <mat4.h>
#include "config.h"
#include "error.h"
#include "frame.h"
#include "init.h"
#include "command.h"
#include "memory.h"
#include "pipeline.h"
#include "vertex.h"


static uint32_t WIDTH = 800;
static uint32_t HEIGHT = 600;
static bool frameBufferResized = FALSE;

static void ResizeCallback(GLFWwindow* window, int width, int height) {
    WIDTH = width;
    HEIGHT = height;
    frameBufferResized = TRUE;
}

//sprite ID
typedef i32 SpriteHandle;


SpriteHandle CreateSprite(sm_vec3f pos, sm_vec3f size);
ErrorCode DestroySprite(SpriteHandle s);

ErrorCode PushBuffer(void* buf);
sm_mat4f* GetModel(SpriteHandle s);
u32 GetNum();

typedef struct {
    VulkanDevice* d;
    VulkanContext* context; 
    VulkanCommand* cmd;
    StaticBuffer* pos;
    StaticBuffer* uvs;
    StaticBuffer* index;
    VulkanShader* shader;
    VulkanPipelineConfig* config;
    VulkanPipeline* pipeline;
    RenderPass* pass;
    SwapChain* swap;
    UniformHandles* uniforms;
} RenderState;

void DrawFrame(RenderState r, unsigned int frame);



#endif