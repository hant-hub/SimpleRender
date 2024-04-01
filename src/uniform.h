#ifndef SR_UNIFORM_H
#define SR_UNIFORM_H

#include "error.h"
#include "mat4.h"
#include "init.h"
#include "pipeline.h"
#include "memory.h"
#include <vulkan/vulkan_core.h>

#include "texture.h"

typedef struct {
    sm_mat4f model;
    sm_mat4f view;
    sm_mat4f proj;
} UniformObj;

typedef struct {
    VkBuffer bufs[SR_MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory mem[SR_MAX_FRAMES_IN_FLIGHT];
    void* objs[SR_MAX_FRAMES_IN_FLIGHT];
} UniformHandles;

ErrorCode CreateUniformBuffer(UniformHandles* handles, Texture* t, VulkanPipelineConfig* config, VulkanDevice* d);
void DestroyUniformBuffer(VkDevice d, UniformHandles* handles);


#endif
