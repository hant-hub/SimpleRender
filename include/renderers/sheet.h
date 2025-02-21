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
} Sh_Camera;

typedef struct {
    sm_mat4f model;
    sm_vec2f uvoffset;
    sm_vec2f uvscale;
    u32 texture;
} __attribute__ ((aligned(sizeof(sm_vec4f)))) SheetPack;

typedef struct {
    StaticBuffer verts;
    StaticBuffer index;
    VulkanShader shader;
    VulkanPipelineConfig config;
    VulkanPipeline pipeline;
    DynamicBuffer uniforms[SR_MAX_FRAMES_IN_FLIGHT];
    DynamicBuffer modelBuf[SR_MAX_FRAMES_IN_FLIGHT];
    Sh_Camera cam;
    SheetEntry denseSetVals[SR_MAX_INSTANCES];
    u32 denseSetIdx[SR_MAX_INSTANCES];
    i32 sparseSet[SR_MAX_INSTANCES];
    u32 denseSize;
    Attachment depth;
} SheetRenderer;


ErrorCode SheetInit(SheetRenderer* r, RenderPass* p, u32 subpass, Sh_Camera c, u32 textureSlots);
void SheetDestroy(SheetRenderer* r);

SheetHandle CreateSpriteSh(SheetRenderer* r, sm_vec2f pos, sm_vec2f size, u32 tex, u32 layer);
ErrorCode DestroySpriteSh(SheetRenderer* r, SheetHandle s);

SheetEntry* GetSpriteSh(SheetRenderer* r, SheetHandle s);
u32 GetNumSh(SheetRenderer* r);
Sh_Camera* GetCamSh(SheetRenderer* r);

ErrorCode SHSetTextureSlot(SheetRenderer* r, Texture* t, u32 index);
ErrorCode SHSetTextureSlots(SheetRenderer* r, Texture* t, u32 number);

void SheetDrawFrame(SheetRenderer* r, PresentInfo* p, unsigned int frame);
ErrorCode SheetGetSubpass(SubPass* s, Attachment* a, u32 start);


#endif
