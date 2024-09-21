#ifndef SR_SPRITE_H
#define SR_SPRITE_H


#include "common.h"
#include <mat4.h>
#include "config.h"
#include "error.h"
#include "init.h"
#include "command.h"
#include "log.h"
#include "memory.h"
#include "pipeline.h"



//sprite ID
typedef i32 SpriteHandle;



typedef struct {
    sm_mat4f model;
//    i32 texture;
} SpriteEntry;

typedef struct {
    VulkanCommand cmd;
    StaticBuffer verts;
    StaticBuffer index;
    VulkanShader shader;
    VulkanPipelineConfig config;
    VulkanPipeline pipeline;
    RenderPass pass;
    SwapChain swap;
    BufferHandle uniforms;
    SpriteEntry denseSetVals[SR_MAX_INSTANCES];
    u32 denseSetIdx[SR_MAX_INSTANCES];
    i32 sparseSet[SR_MAX_INSTANCES];
    u32 denseSize;
} RenderState;


SpriteHandle CreateSprite(RenderState* r, sm_vec3f pos, sm_vec3f size);
ErrorCode DestroySprite(RenderState* r, SpriteHandle s);

ErrorCode PushBuffer(RenderState* r, void* buf);
sm_mat4f* GetModel(RenderState* r, SpriteHandle s);
u32 GetNum(RenderState* r);

void DrawFrame(RenderState* r, unsigned int frame);



#endif
