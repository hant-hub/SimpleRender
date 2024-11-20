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
#define MAX_CHARS 10000


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
    u32 chars;
    DynamicBuffer uniforms;
    DynamicBuffer indicies;
    DynamicBuffer verts;
} TextRenderer;

ErrorCode LoadFont(FontData* font);

ErrorCode TextInit(TextRenderer* r);
void TextDestroy(TextRenderer* r);

ErrorCode SetText(TextRenderer* r, const char* text, u32 textlen, sm_vec2f pos, float scale);
ErrorCode ReplaceText(TextRenderer* r, const char* text, u32 start, u32 end);
ErrorCode AppendText(TextRenderer* r, const char* text, u32 textlen, sm_vec2f pos, float scale);

void TextDrawFrame(TextRenderer* r, PresentInfo* p, u32 frame);


#define TextUpdate(r, t) \
    UpdateText(r, t, sizeof(t) - 1, (sm_vec2f){0,0}, 1)


#endif
