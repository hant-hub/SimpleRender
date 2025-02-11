#ifndef SR_SHEET_H
#define SR_SHEET_H


#include "angles.h"
#include "common.h"
#include <mat4.h>
#include <vec2.h>
#include "config.h"
#include "error.h"
#include "frame.h"
#include "init.h"
#include "memory.h"
#include "pipeline.h"
#include "texture.h"


#define MAX_LAYERS 100
#define SR_MAX_INSTANCES 1500
#define SR_SHEET_ATTACHMENT_NUM 1

//sprite ID
typedef i32 SheetHandle;


typedef struct {
    sm_vec2f pos;
    sm_vec2f size;
    sm_vec2f scale;
    sm_vec2f selection;
    Radian rotation;
    u32 layer;
    u32 texture;
} SheetEntry;

typedef struct {
    sm_vec2f pos;
    sm_vec2f size;
    Radian rotation;
} Camera;

typedef struct {
    sm_mat4f model;
    sm_vec2f uvoffset;
    sm_vec2f uvscale;
    u32 texture;
} __attribute__ ((aligned(sizeof(sm_vec4f)))) SpritePack;

typedef struct {
    StaticBuffer verts;
    StaticBuffer index;
    VulkanShader shader;
    VulkanPipelineConfig config;
    VulkanPipeline pipeline;
    DynamicBuffer uniforms[SR_MAX_FRAMES_IN_FLIGHT];
    DynamicBuffer modelBuf[SR_MAX_FRAMES_IN_FLIGHT];
    Camera cam;
    SheetEntry denseSetVals[SR_MAX_INSTANCES];
    u32 denseSetIdx[SR_MAX_INSTANCES];
    i32 sparseSet[SR_MAX_INSTANCES];
    u32 denseSize;
    Attachment depth;
} SpriteRenderer;


ErrorCode SpriteInit(SpriteRenderer* r, RenderPass* p, u32 subpass, Camera c, uint textureSlots);
void SpriteDestroy(SpriteRenderer* r);

SheetHandle CreateSprite(SpriteRenderer* r, sm_vec2f pos, sm_vec2f size, u32 tex, u32 layer);
ErrorCode DestroySprite(SpriteRenderer* r, SheetHandle s);

ErrorCode PushBuffer(SpriteRenderer* r, void* buf);
SheetEntry* GetSprite(SpriteRenderer* r, SheetHandle s);
u32 GetNum(SpriteRenderer* r);
Camera* GetCam(SpriteRenderer* r);

ErrorCode SetTextureSlot(SpriteRenderer* r, Texture* t, u32 index);
ErrorCode SetTextureSlots(SpriteRenderer* r, Texture* t, u32 number);

void SpriteDrawFrame(SpriteRenderer* r, PresentInfo* p, unsigned int frame);
ErrorCode SpriteGetSubpass(SubPass* s, Attachment* a, u32 start);


#endif
