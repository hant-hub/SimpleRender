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


ErrorCode CreateImage(VulkanDevice* d, VulkanCommand* c, Texture* t);
void DestroyImage(VkDevice d, Texture* t);

#endif
