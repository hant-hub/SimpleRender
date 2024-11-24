#ifndef SR_TEXT_H
#define SR_TEXT_H

#include "config.h"
#include "error.h"
#include "frame.h"
#include "memory.h"
#include "pipeline.h"
#include "texture.h"
#include "vec2.h"

#define NUM_GLYPHS 128
#define MAX_CHARS 100000


typedef struct {
    Texture atlas;
    //position in atlas
    sm_vec2i pos[NUM_GLYPHS];
    //size on atlas
    sm_vec2i size[NUM_GLYPHS];
    //bearing
    sm_vec2i offset[NUM_GLYPHS]; 
    //kerning stuff
    int advance[NUM_GLYPHS];
    //atlas size
    sm_vec2i texsize;
} FontData;

typedef struct {
    FontData fdata;
    VulkanShader shader;
    VulkanPipelineConfig config;
    VulkanPipeline pipeline;
    RenderPass pass;
    SwapChain swap;
    StaticBuffer font;
    DynamicBuffer uniforms;
    DynamicBuffer indicies;
    DynamicBuffer verts;
    u32 chars;
} TextRenderer;

ErrorCode LoadFont(FontData* font);

ErrorCode AppendText(TextRenderer* r, const char* text, u32 textLen, sm_vec2f pos, float scale);
ErrorCode ClearText(TextRenderer* r);

ErrorCode TextInit(TextRenderer* r);
void TextDestroy(TextRenderer* r);


void TextDrawFrame(TextRenderer* r, PresentInfo* p, u32 frame);



#endif
