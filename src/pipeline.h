#ifndef SR_PIPE_H
#define SR_PIPE_H

#include "error.h"
#include "init.h"
#include "log.h"
#include "error.h"
#include "util.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

typedef struct {
    VkShaderModule vertex;
    VkShaderModule fragment;
} VulkanShader;

typedef struct {
} VulkanPipeline;


ErrorCode CreateShaderProg(VkDevice d, const char* vertex, const char* frag, VulkanShader* s);
ErrorCode CreatePipeline(VulkanDevice* d, VulkanContext* c, VulkanShader* s, VulkanPipeline* p);





#endif
