#include "Vulkan.h"
#include "error.h"
#include "util.h"
#include "state.h"
#include "init.h"
#include "frame.h"
#include "vert/types.h"
#include "vertex.h"
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>



static const uint32_t WIDTH = 1920;
static const uint32_t HEIGHT = 1080;


const int MAX_FRAMES_IN_FLIGHT = 2;
static int minimized = 0;
static int framebufferResized = 0;

static void WindowMinimizedCallback(GLFWwindow* w, int iconified) {
    if (iconified == GLFW_TRUE) {
        minimized = 1;
    } else if (iconified == GLFW_FALSE) {
        minimized = 0;
    }

}

static void framebufferResizeCallback(GLFWwindow* w, int width, int height) {
    framebufferResized = 1;
}


inline static void CloseGLFW(GLFWwindow *w) {
    glfwDestroyWindow(w);
    glfwTerminate();
}

int main() {

    VulkanDevice d = {0};
    glfwInit();
    //create Window
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    d.w = glfwCreateWindow(WIDTH, HEIGHT, "Renderer", glfwGetPrimaryMonitor(), NULL);
    glfwSetFramebufferSizeCallback(d.w, framebufferResizeCallback);
    glfwSetWindowIconifyCallback(d.w, WindowMinimizedCallback);
    int frameBufferResized = 0;
    glfwSetWindowUserPointer(d.w, &frameBufferResized);

    ErrorCode result = InitVulkan(&d);
    if (result == Error) {
        exit(EXIT_FAILURE);
    }

    SwapChain s = {0};
    result = CreateSwapChain(&d, &s, COMPLETE); 
    if (result == Error) {
        ExitVulkan(&d, COMPLETE);
        exit(EXIT_FAILURE);
    }

    Pipeline p = {0};
    result = CreatePipeline(&d, &s, &p, COMPLETE);
    if (result == Error) {
        DestroySwapChain(&d, &s, COMPLETE);
        ExitVulkan(&d, COMPLETE);
        exit(EXIT_FAILURE);
    }




    
    Command c = {0};
    result = CreateCommandObjects(&c, MAX_FRAMES_IN_FLIGHT, &d);
    if (result == Error) {
        DestroyPipeline(&d, &s, &p, COMPLETE);
        DestroySwapChain(&d, &s, COMPLETE);
        ExitVulkan(&d, COMPLETE);
        exit(EXIT_FAILURE);
    }
    
    VertexBuffer vertBuffer;
    result = CreateVertexBuffer(&d, &c, &vertBuffer); 
    if (result == Error) {
        vkDestroyCommandPool(d.logical.device, c.pool, NULL);
        DestroyPipeline(&d, &s, &p, COMPLETE);
        DestroySwapChain(&d, &s, COMPLETE);
        ExitVulkan(&d, COMPLETE);
        exit(EXIT_FAILURE);
    }

    IndexBuffer indexBuffer;
    result = CreateIndexBuffer(&d, &c, &indexBuffer);
    if (result == Error) {
        DestroyBuffer(d.logical.device, &vertBuffer.buffer, &vertBuffer.memory);
        vkDestroyCommandPool(d.logical.device, c.pool, NULL);
        DestroyPipeline(&d, &s, &p, COMPLETE);
        DestroySwapChain(&d, &s, COMPLETE);
        ExitVulkan(&d, COMPLETE);
        exit(EXIT_FAILURE);
    }

    SyncObjects sync = {0};
    result = CreateSyncObjects(&d, &c, &sync, MAX_FRAMES_IN_FLIGHT);
    if (result == Error) {
        DestroyBuffer(d.logical.device, &vertBuffer.buffer, &vertBuffer.memory);
        vkDestroyCommandPool(d.logical.device, c.pool, NULL);
        DestroyPipeline(&d, &s, &p, COMPLETE);
        DestroySwapChain(&d, &s, COMPLETE);
        ExitVulkan(&d, COMPLETE);
        exit(EXIT_FAILURE);
    }


    uint32_t currentFrame = 0; 
    while (!glfwWindowShouldClose(d.w)){

        ErrorCode result = DrawFrame(&d, &s, &c, &p, &vertBuffer, &indexBuffer, &sync, &framebufferResized, &minimized, currentFrame);
        if (result == Error)
            break;
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    vkDeviceWaitIdle(d.logical.device);
    DestroySyncObjects(&d, &sync);
    DestroyBuffer(d.logical.device, &vertBuffer.buffer, &vertBuffer.memory);
    DestroyBuffer(d.logical.device, &indexBuffer.buffer, &indexBuffer.memory);
    vkDestroyCommandPool(d.logical.device, c.pool, NULL);
    free(c.buffers);
    DestroyPipeline(&d, &s, &p, COMPLETE);
    DestroySwapChain(&d, &s, COMPLETE);
    ExitVulkan(&d, COMPLETE);
    return EXIT_SUCCESS;
}
