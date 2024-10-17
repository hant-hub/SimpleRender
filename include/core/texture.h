#ifndef SR_TEXTURE_H
#define SR_TEXTURE_H
#include "vulkan/vulkan.h"
#include "command.h"
#include "memory.h"
#include <vulkan/vulkan_core.h>



typedef struct {
    Image image;
    VkSampler sampler;
} Texture;

ErrorCode LoadTexture(VulkanCommand* c, Texture* t, const char* path);
ErrorCode CreateImage(VulkanCommand* c, Image* t, size_t width, size_t height);
void DestroyTexture(Texture* t);
void DestroyImage(Image* t);

#endif
