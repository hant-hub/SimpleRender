#ifndef SR_SPRITE_H
#define SR_SPRITE_H


#include "angles.h"
#include "common.h"
#include <mat4.h>
#include <vec2.h>
#include "config.h"
#include "error.h"
#include "init.h"
#include "command.h"
#include "log.h"
#include "memory.h"
#include "pipeline.h"
#include "texture.h"


#define MAX_LAYERS 100

//sprite ID
typedef i32 SpriteHandle;


typedef struct {
    sm_vec2f pos;
    sm_vec2f size;
    u32 layer;
    Radian rotation;
    u32 texture;
} SpriteEntry;

typedef struct {
    sm_vec2f pos;
    sm_vec2f size;
    Radian rotation;
} Camera;

typedef struct {
    sm_mat4f model;
    u32 texture;
} __attribute__ ((aligned(sizeof(sm_vec4f)))) SpritePack;

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
    BufferHandle modelBuf;
    Camera cam;
    SpriteEntry denseSetVals[SR_MAX_INSTANCES];
    u32 denseSetIdx[SR_MAX_INSTANCES];
    i32 sparseSet[SR_MAX_INSTANCES];
    u32 denseSize;
} RenderState;


ErrorCode SpriteInit(RenderState* r, Camera c, uint textureSlots);
void SpriteDestroy(RenderState* r);

SpriteHandle CreateSprite(RenderState* r, sm_vec2f pos, sm_vec2f size, u32 tex, u32 layer);
ErrorCode DestroySprite(RenderState* r, SpriteHandle s);

ErrorCode PushBuffer(RenderState* r, void* buf);
SpriteEntry* GetSprite(RenderState* r, SpriteHandle s);
u32 GetNum(RenderState* r);
Camera* GetCam(RenderState* r);

ErrorCode SetTextureSlot(RenderState* r, Texture* t, u32 index);
ErrorCode SetTextureSlots(RenderState* r, Texture* t, u32 number);

void DrawFrame(RenderState* r, unsigned int frame);



#endif
