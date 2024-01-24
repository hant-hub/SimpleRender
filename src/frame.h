#ifndef FRAME_H
#define FRAME_H
#include "GLFW/glfw3.h"
#include "error.h"
#include "state.h"
#include "init.h"
#include <stdint.h>

ErrorCode DrawFrame(VulkanDevice* d, SwapChain* s, Command* c, Pipeline* p,
                    SyncObjects* sync, int* framebufferResized, uint32_t currentFrame);

#endif
