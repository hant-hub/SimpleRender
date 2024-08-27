#ifndef FRAME_H
#define FRAME_H

#include "command.h"
#include "common.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include "mat4.h"
#include "pipeline.h"
#include "texture.h"
#include "util.h"
#include "vertex.h"
#include "sprite.h"
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <string.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>


static bool frameBufferResized;


void DrawFrame(VulkanDevice* device, VulkanCommand* cmd, GeometryBuffer* buffer, VulkanContext* context, VulkanShader* s,
                      VulkanPipelineConfig* config, RenderPass* pass, SwapChain* swapchain, VulkanPipeline* pipe, UniformHandles* uniforms, unsigned int frame);



#endif
