#include "text.h"
#include "ft2build.h"
#include "texture.h"
#include <vulkan/vulkan_core.h>
#include FT_FREETYPE_H


//should only run once to load all fonts needed upon startup
ErrorCode LoadFont(const char* fontname, FontData* font) {
    FT_Library library;

    if (FT_Init_FreeType(&library)) {
        SR_LOG_ERR("Failed to load freetype");
        return SR_LOAD_FAIL;
    }

    FT_Face face;
    if (FT_New_Face(library, fontname, 0, &face)) {
        SR_LOG_ERR("Failed to load font face");
        return SR_LOAD_FAIL;
    }
    FT_Set_Pixel_Sizes(face, 0, SR_TEXT_SIZE);

    int max_dim = (1 + (face->size->metrics.height >> 6)) * ceilf(sqrtf(NUM_GLYPHS));
	int tex_width = 1;
	while(tex_width < max_dim) tex_width <<= 1;
	int tex_height = tex_width;

    char* buffer = calloc(tex_width * tex_height, 1);

    int pen_x = 0;
    int pen_y = 0;
    for (int i = 0; i < NUM_GLYPHS; i++) {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT)) {
            SR_LOG_ERR("Failed to load character");
            return SR_LOAD_FAIL;
        }
        FT_Bitmap* bmp = &face->glyph->bitmap;

        if (pen_x + bmp->width >= tex_width) {
            pen_x = 0;
            pen_y += (face->size->metrics.height >> 6) + 1;
        }

        for (int row = 0; row < bmp->rows; row++) {
            for (int col = 0; col < bmp->width; col++) {
                int x = pen_x + col;
                int y = pen_y + row;
                buffer[y * tex_width + x] = bmp->buffer[row * bmp->width + col];
            }
        }

        font->pos[i] = (sm_vec2i){pen_x, pen_y};
        font->size[i] = (sm_vec2i){bmp->width, bmp->rows};
        font->offset[i] = (sm_vec2i){face->glyph->bitmap_left, face->glyph->bitmap_top};
        font->advance[i] = face->glyph->advance.x>>6;
        pen_x += bmp->width + 1;
    }
    FT_Done_Face(face);

    CreateTexture(&font->atlas, (TextureConfig){
                .width = tex_width,
                .height = tex_height,
                .channels = 1,
                .format = VK_FORMAT_R8_SRGB,
                .accessmode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
            }, buffer);

    font->texsize = (sm_vec2i){tex_width, tex_height};
    free(buffer);
    return SR_NO_ERROR;
}
