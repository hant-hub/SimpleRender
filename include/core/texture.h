#ifndef SR_TEXTURE_H
#define SR_TEXTURE_H
#include "error.h"
#include <vulkan/vulkan_core.h>


typedef struct {
    VkImage image;
    VkImageView view;
    VkDeviceMemory mem;
} Image;

typedef struct {
    VkImage image;
    VkImageView view;
    VkDeviceMemory mem;
    char* buf; //byte array
} DynamicImage;

typedef struct {
    Image image;
    VkSampler sampler;
} Texture;

typedef struct {
    Texture t;
    VkFormat format;
    size_t size;
    char* data;
} DynamicTexture;

typedef struct {
    size_t width;
    size_t height;
    VkImageTiling tiling;
    VkImageUsageFlags usage;
    VkImageAspectFlags mask;
    VkFormat format;
} ImageConfig;

typedef struct {
    size_t width;
    size_t height;
    size_t channels;
    VkFormat format;
    VkSamplerAddressMode accessmode;
    VkFilter filter;
    VkBool32 anisotropy;
}TextureConfig;

ErrorCode LoadTexture(Texture* t, const char* path);
ErrorCode CreateTexture(Texture* t, TextureConfig config, void* buf);
ErrorCode CreateDynTexture(DynamicTexture* t, TextureConfig config);

ErrorCode CreateImage(Image* t, ImageConfig config);
ErrorCode CreateDynImage(DynamicImage* t, ImageConfig config);

ErrorCode BeginUpdateDynTexture(DynamicTexture* t);
ErrorCode EndUpdateDynTexture(DynamicTexture* t);

void DestroyTexture(Texture* t);
void DestroyImage(Image* t);
void DestroyDynTexture(DynamicTexture* t);
void DestroyDynImage(Image* t);
#endif
