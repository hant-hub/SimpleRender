#ifndef SR_MULTI_SPRITE_H
#define SR_MULTI_SPRITE_H


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


SpriteHandle MultiCreateSprite(sm_vec3f pos, sm_vec3f size);
ErrorCode MultiDestroySprite(SpriteHandle s);

ErrorCode MultiPushBuffer(void* buf);
sm_mat4f* MultiGetModel(SpriteHandle s);
u32 MultiGetNum();

typedef struct {
    VulkanCommand* cmd;
    StaticBuffer* pos;
    StaticBuffer* uvs;
    StaticBuffer* index;
    VulkanShader* shader;
    VulkanPipelineConfig* config;
    VulkanPipeline* pipeline;
    RenderPass* pass;
    SwapChain* swap;
    BufferHandle* uniforms;
} RenderState;

void DrawFrameMultiSprite(RenderState r, unsigned int frame);



#endif
