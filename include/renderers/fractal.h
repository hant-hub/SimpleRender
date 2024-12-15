#ifndef SR_FRACTAL_H
#define SR_FRACTAL_H

#include "memory.h"
#include "config.h"
#include "pipeline.h"


typedef struct {
    StaticBuffer verts;
    StaticBuffer index;
    VulkanShader shader;
    VulkanPipelineConfig config;
    VulkanPipeline pipeline;
    DynamicBuffer uniforms[SR_MAX_FRAMES_IN_FLIGHT];
} FractalRenderer;





#endif
