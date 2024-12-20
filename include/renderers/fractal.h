#ifndef SR_FRACTAL_H
#define SR_FRACTAL_H

#include "frame.h"
#include "memory.h"
#include "config.h"
#include "pipeline.h"


typedef struct {
    StaticBuffer verts;
    StaticBuffer index;
    VulkanShader shader;
    VulkanPipelineConfig config;
    VulkanPipeline pipeline;
    DynamicBuffer uniform[SR_MAX_FRAMES_IN_FLIGHT];
} FractalRenderer;


ErrorCode FractalInit(FractalRenderer* r, RenderPass* p, u32 subpass);
void FractalDrawFrame(FractalRenderer* r, PresentInfo* p, u32 frame);
void FractalDestroy(FractalRenderer* r);

ErrorCode FractalGetSubpass(SubPass* s, Attachment* a, u32 start);

#endif
