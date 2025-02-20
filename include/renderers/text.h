#ifndef SR_TEXT_H
#define SR_TEXT_H

#include "config.h"
#include "error.h"
#include "frame.h"
#include "memory.h"
#include "pipeline.h"
#include "texture.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

#define NUM_GLYPHS 128
#define MAX_CHARS 100000

#define SR_TEXT_NUM_ATTACHMENTS 1

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
    //preserve specified height of font
    float charheight;
} Font;

typedef struct {
    Font* fdata;
    VulkanShader shader;
    VulkanPipelineConfig config;
    VulkanPipeline pipeline;
    SwapChain swap;
    DynamicBuffer vertuniforms;
    DynamicBuffer indicies;
    DynamicBuffer verts;
    u32 chars;
    sm_vec2f textarea;
    sm_vec2f appendPos;
    sm_vec4f currentTextColor;
} TextRenderer;

ErrorCode LoadFont(const char* fontname, u32 size, Font* font);
void DestroyFont(Font* f);

ErrorCode AppendText(TextRenderer* r, const char* text, u32 textLen, sm_vec2f pos, float layer, float scale);
ErrorCode ClearText(TextRenderer* r);
ErrorCode SetColor(TextRenderer* r, sm_vec3f color);
ErrorCode SetArea(TextRenderer* r, sm_vec2f area);

sm_vec2f GetTextPos(TextRenderer* r);

//Expensive so use with care
sm_vec2f GetTextSize(Font* f, const char* text, u32 textlen);

//Better reuse
ErrorCode SetFont(TextRenderer* r, Font* f);

//TODO replace font path with font data
ErrorCode TextInit(TextRenderer* r, Font* f, RenderPass* p, u32 subpass);
void TextDestroy(TextRenderer* r);

void TextDrawFrame(TextRenderer* r, PresentInfo* p, u32 frame);
ErrorCode TextGetSubpass(SubPass* s, Attachment* a, u32 start);


#endif
