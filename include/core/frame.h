#ifndef SR_FRAME_H
#define SR_FRAME_H


#include "common.h"
#include "config.h"
#include "init.h"
#include "pipeline.h"
#include <vulkan/vulkan_core.h>

typedef struct {
    //This is the renderpass which outputs the final image
    RenderPass* p;
    SwapChain swapchain;
    u32 imageIndex;
    VkSemaphore imageAvalible[SR_MAX_FRAMES_IN_FLIGHT];
    VkSemaphore renderFinished[SR_MAX_FRAMES_IN_FLIGHT];
    VkFence inFlight[SR_MAX_FRAMES_IN_FLIGHT];
} PresentInfo;


ErrorCode InitPresent(PresentInfo* p, RenderPass* r); 
ErrorCode GetFrame(PresentInfo* p, u32 frame);
ErrorCode PresentFrame(PresentInfo* p, u32 frame);
void DestroyPresent(PresentInfo* p);

#endif
