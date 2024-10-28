#ifndef SR_TEXTURE_H
#define SR_TEXTURE_H
#include "vulkan/vulkan.h"
#include "command.h"
#include "memory.h"
#include <vulkan/vulkan_core.h>


typedef struct {
    VkImage image;
    VkImageView view;
    VkDeviceMemory mem;
} Image;

typedef struct {
    Image image;
    VkSampler sampler;
} Texture;


typedef struct {
    size_t width;
    size_t height;
    VkImageTiling tiling;
    VkImageUsageFlags usage;
    VkImageAspectFlags mask;
    VkFormat format;
} ImageConfig;

ErrorCode LoadTexture(VulkanCommand* c, Texture* t, const char* path);
ErrorCode CreateImage(VulkanCommand* c, Image* t, ImageConfig config);
void DestroyTexture(Texture* t);
void DestroyImage(Image* t);

#endif
