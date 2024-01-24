#ifndef INIT_H
#define INIT_H
#include "lib/SimpleMath/src/include/misc.h"
#include "error.h"
#include "init/instance.h"
#include "init/physicaldevice.h"
#include "init/imageviews.h"
#include "init/pipeline.h"
#include "init/framebuffer.h"
#include "init/logicaldevice.h"
#include "state.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include "Vulkan.h"
#include "state.h"


ErrorCode InitVulkan(VulkanDevice* deviceData);
void ExitVulkan(VulkanDevice* deviceData, InitStage stage);

ErrorCode CreateSwapChain(VulkanDevice* d, SwapChain* s, InitStage stage);
void DestroySwapChain(VulkanDevice* d, SwapChain* s, InitStage stage);


void DestroyPipeline(VulkanDevice* d, SwapChain* s, Pipeline* p, InitStage stage);
ErrorCode CreatePipeline(VulkanDevice* d, SwapChain* s, Pipeline* p, InitStage stagg);

ErrorCode CreateCommandObjects(Command* c, uint32_t bufferCount, VulkanDevice* d);
ErrorCode RecordCommands(VkCommandBuffer* commandBuffer, Pipeline* p, SwapChain* s, uint32_t imageIndex);

ErrorCode CreateSyncObjects(VulkanDevice* d, Command* c, SyncObjects* s, uint32_t MAX_FRAMES_IN_FLIGHT);
void DestroySyncObjects(VulkanDevice* d, SyncObjects* s);

#endif
