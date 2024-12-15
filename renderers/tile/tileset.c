#include "error.h"
#include "log.h"
#include "texture.h"
#include "tile.h"
#include "stb_image.h"
#include <vulkan/vulkan_core.h>



ErrorCode TileLoadSetDir(TileSet* t) {
    return SR_NO_ERROR;
}

ErrorCode TileLoadSetFile(TileSet* t, const char* filename, int width, int height) {
    int imgwidth, imgheight;
    int channels;
    unsigned char* buffer = stbi_load(filename, &imgwidth, &imgheight, &channels, 3);
    
    SR_LOG_DEB("Channels: %d", channels);

    CreateTexture(&t->atlas, (TextureConfig){
            .width = imgwidth,
            .height = imgheight,
            .channels = channels,
            .format = VK_FORMAT_R8G8B8_SRGB,
            .accessmode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .filter = VK_FILTER_NEAREST,
            .anisotropy = VK_FALSE
            }, buffer);

    t->height = imgheight;
    t->width = imgwidth;
    t->tilewide = width;
    t->tileheight = height;
    stbi_image_free(buffer);
    return SR_NO_ERROR;
}
