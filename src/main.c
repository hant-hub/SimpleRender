#include "error.h"
#include "init.h"
#include "log.h"
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <vulkan/vulkan_core.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

static void ExitProg(GLFWwindow* window, VulkanContext* context) {
#ifdef DEBUG
    DestroyDebugMessenger(context);
#endif
    
    vkDestroyInstance(context->instance, NULL);

    glfwDestroyWindow(window);
    glfwTerminate();
}

int main() {

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "SimpleRender", NULL, NULL);
    VulkanContext context;
    ErrorCode result = CreateInstance(&context);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context);

    


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    ExitProg(window, &context);
    return 0;
}
