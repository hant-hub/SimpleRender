#ifndef SR_TILE_H
#define SR_TILE_H



#include "config.h"
#include "frame.h"
#include "init.h"
#include "memory.h"
#include "pipeline.h"
#include "texture.h"
#include "vec2.h"

#define SR_TILE_ATTACHMENT_NUM 0

typedef struct {
    Texture atlas;
    u32 width;
    u32 height;
    u32 tilewide;
    u32 tileheight;
} TileSet;


typedef struct {
    TileSet tileset;
    VulkanShader shader;
    VulkanPipelineConfig config;
    VulkanPipeline pipeline;
    StaticBuffer verts;
    StaticBuffer indicies;
    DynamicBuffer tiledata[SR_MAX_FRAMES_IN_FLIGHT];
    DynamicBuffer uniform[SR_MAX_FRAMES_IN_FLIGHT];
    sm_vec2f size; //how much of screen
} TileRenderer;

ErrorCode TileLoadSetFile(TileSet* t, const char* filename, int width, int height);
void TileSetData(TileRenderer* r, int* data);

void TileDrawFrame(TileRenderer* r, PresentInfo* p, u32 frame);
ErrorCode TileInit(TileRenderer* r, RenderPass* p, u32 subpass, u32 width, u32 height);
ErrorCode TileGetSubpass(SubPass* s, Attachment* a, u32 start);
void DestroyTile(TileRenderer* r);
#endif
