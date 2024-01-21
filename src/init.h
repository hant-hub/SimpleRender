#ifndef INIT_H
#define INIT_H
#include "instance.h"
#include "physicaldevice.h"
#include "swapchain.h"
#include "imageviews.h"
#include "pipeline.h"
#include "framebuffer.h"
#include "command.h"
#include "logicaldevice.h"
#include "state.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include "Vulkan.h"
#include "state.h"


void InitVulkan(VulkanState* state);
void ExitVulkan(VulkanState* state);




#endif
