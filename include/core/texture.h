#ifndef SR_TEXTURE_H
#define SR_TEXTURE_H
#include "vulkan/vulkan.h"
#include "command.h"
#include <vulkan/vulkan_core.h>


typedef struct {
    VkImage image;
    VkImageView view;
    VkSampler sampler;

    VkDeviceMemory mem;
} Texture;


ErrorCode CreateImage(VulkanCommand* c, Texture* t, const char* path);
void DestroyImage(Texture* t);

#endif
