#ifndef SR_FRACTAL_H
#define SR_FRACTAL_H

#include "frame.h"
#include "memory.h"
#include "config.h"
#include "pipeline.h"
#include "vec2.h"
#include "vec3.h"


typedef struct {
    StaticBuffer verts;
    StaticBuffer index;
    VulkanShader shader;
    VulkanPipelineConfig config;
    VulkanPipeline pipeline;
    DynamicBuffer uniform[SR_MAX_FRAMES_IN_FLIGHT];
} FractalRenderer;


ErrorCode FractalInit(FractalRenderer* r, char* prog, RenderPass* p, u32 subpass);
void FractalDrawFrame(FractalRenderer* r, PresentInfo* p, u32 frame);
void FractalDestroy(FractalRenderer* r);

void FractalSetZoom(FractalRenderer* r, float z);
void FractalSetPos(FractalRenderer* r, sm_vec2f pos);
void FractalSetColor(FractalRenderer* r, sm_vec3f c);
void FractalSetInit(FractalRenderer* r, sm_vec2f init);

ErrorCode FractalGetSubpass(SubPass* s, Attachment* a, u32 start);

#endif
