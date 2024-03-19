#ifndef SR_TEXTURE_H
#define SR_TEXTURE_H
#include "vulkan/vulkan.h"
#include <vulkan/vulkan_core.h>


typedef struct {
    VkImage image;

    VkDeviceMemory mem;
} Texture;



#endif
