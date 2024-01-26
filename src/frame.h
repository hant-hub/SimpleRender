#ifndef FRAME_H
#define FRAME_H
#include "GLFW/glfw3.h"
#include "error.h"
#include "state.h"
#include "init.h"
#include "vert/types.h"
#include <stdint.h>

ErrorCode DrawFrame(VulkanDevice* d, SwapChain* s, Command* c, Pipeline* p, VertexBuffer* vb, IndexBuffer* ib,
                    SyncObjects* sync, int* framebufferResized, int* minimized, uint32_t currentFrame);

#endif
