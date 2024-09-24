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
#include "texture.h"



//sprite ID
typedef i32 SpriteHandle;



struct SpriteEntry {
    sm_mat4f model;
    u32 texture;
} __attribute__ ((aligned(sizeof(sm_vec4f))));

typedef struct SpriteEntry SpriteEntry;

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
    Texture test1;
    Texture test2;
    SpriteEntry denseSetVals[SR_MAX_INSTANCES];
    u32 denseSetIdx[SR_MAX_INSTANCES];
    i32 sparseSet[SR_MAX_INSTANCES];
    u32 denseSize;
} RenderState;


ErrorCode SpriteInit(RenderState* r);
void SpriteDestroy(RenderState* r);

SpriteHandle CreateSprite(RenderState* r, sm_vec3f pos, sm_vec3f size);
ErrorCode DestroySprite(RenderState* r, SpriteHandle s);

ErrorCode PushBuffer(RenderState* r, void* buf);
sm_mat4f* GetModel(RenderState* r, SpriteHandle s);
u32 GetNum(RenderState* r);

void DrawFrame(RenderState* r, unsigned int frame);



#endif
