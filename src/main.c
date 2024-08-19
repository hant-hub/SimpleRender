#include "command.h"
#include "config.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include "mat4.h"
#include "pipeline.h"
#include "sprite.h"
#include "texture.h"
#include "util.h"
#include "vec3.h"
#include "vertex.h"
#include "uniform.h"
#include "frame.h"
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

static uint32_t WIDTH = 800;
static uint32_t HEIGHT = 600;


static const Vertex verticies[] = {
//     Position          UV             Color
    {{-0.5f, -0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{ 0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{ 0.5f,  0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f,  0.5f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}
};

static const uint16_t indicies[] = {
    0, 1, 2, 2, 3, 0
};

static void ExitProg(GLFWwindow* window, VulkanContext* context, VulkanDevice* device, SwapChain* swap,
                     VulkanShader* shader, VulkanPipelineConfig* config, RenderPass* r, VulkanPipeline* pipeline, VulkanCommand* cmd, GeometryBuffer* buffer,
                     UniformHandles* uniforms) {
    
    DestroyGeometryBuffer(device->l, buffer);
    DestroyCommand(cmd, device);
    DestroyPipeline(device->l, pipeline);
    DestroyShaderProg(device->l, shader);
    DestroyPipelineConfig(device->l, config);
    DestroyPass(device->l, r);
    DestroyUniformBuffer(device->l, uniforms);
    DestroySwapChain(device->l, swap);
    DestroyDevice(device);
    DestroyContext(context);
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(1);
}

static void ResizeCallback(GLFWwindow* window, int width, int height) {
    WIDTH = width;
    HEIGHT = height;
    frameBufferResized = TRUE;
}


int main() {

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "SimpleRender", NULL, NULL);
    glfwSetFramebufferSizeCallback(window, ResizeCallback);

    VulkanContext context = {0};
    VulkanDevice device = {0};
    VulkanPipelineConfig config = {0};
    RenderPass pass = {0};
    SwapChain swapchain = {0};
    VulkanShader shader = {0};
    VulkanPipeline pipeline = {0};
    VulkanCommand cmd = {0};
    GeometryBuffer buffer = {0};
    UniformHandles uniforms = {0};
    Texture test = {0};

    ErrorCode result = CreateInstance(&context);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);
    context.w = window;

    result = CreateSurface(&context, window);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);

    result = CreateDevices(&device, &context);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);

    result = CreateShaderProg(device.l, "shaders/standard.vert.spv", "shaders/standard.frag.spv", &shader);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);

    result = CreatePipelineConfig(&device, &context, &shader, &config);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);

    result = CreatePass(&pass, &device, &context);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);

    result = CreateSwapChain(&device, &context, &pass, &swapchain, VK_NULL_HANDLE);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);
    
    result = CreatePipeline(&device, &shader, &config, &pipeline, &pass); 
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);

    result = CreateCommand(&cmd, &context, &device);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);

    result = CreateStaticGeometry(&buffer, verticies, indicies, sizeof(verticies), sizeof(indicies), &device, &cmd);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);

    result = CreateImage(&device, &cmd, &test);
    if (result != SR_NO_ERROR) {
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);
    }

    result = CreateUniformBuffer(&uniforms, &test, &config, &device);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);

    SpriteHandle s1 = CreateSprite((sm_vec3f){-0.5f, 0.5f, 0}, (sm_vec3f){1, 1, 0});
    SpriteHandle s2 = CreateSprite((sm_vec3f){0.5f, 0.5f, 0}, (sm_vec3f){1, 1, 0});
    SpriteHandle s3 = CreateSprite((sm_vec3f){0.5f, -0.5f, 0}, (sm_vec3f){1, 1, 0});
    SpriteHandle s4 = CreateSprite((sm_vec3f){-0.5f, -0.5f, 0}, (sm_vec3f){1, 1, 0});


    unsigned int frameCounter = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        frameCounter = frameCounter + 1;
        DrawFrame(&device, &cmd, &buffer, &context, &shader, &config, &pass, &swapchain, &pipeline, &uniforms, frameCounter % SR_MAX_FRAMES_IN_FLIGHT);
//        sm_mat4f* model = GetModel(s1);
//        sm_mat4_f32_translate(model, (sm_vec3f){0.01f, 0.0f, 0.0f});
    }

    vkDeviceWaitIdle(device.l);
    DestroyImage(device.l, &test);
    ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);
    return 0;
}
