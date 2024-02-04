#include "error.h"
#include "init.h"
#include "log.h"
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <vulkan/vulkan_core.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

static void ExitProg(GLFWwindow* window, VulkanContext* context, VulkanDevice* device) {
    
    DestroyDevice(device);
    
    DestroyContext(context);

    glfwDestroyWindow(window);
    glfwTerminate();
}

int main() {

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "SimpleRender", NULL, NULL);

    VulkanContext context = {0};
    VulkanDevice device = {0};

    ErrorCode result = CreateInstance(&context);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device);
    context.w = window;

    result = CreateSurface(&context, window);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device);

    result = CreateDevices(&device, &context);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device);

    


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    ExitProg(window, &context, &device);
    return 0;
}
