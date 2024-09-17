#include "command.h"
#include "common.h"
#include "config.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include "mat4.h"
#include "memory.h"
#include "pipeline.h"
#include "renderers/spriteRenderer/sprite.h"
#include "texture.h"
#include "util.h"
#include "vec2.h"
#include "vec3.h"
#include "vertex.h"
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <vulkan/vulkan_core.h>


static const Vertex verts[] = {
    {{-0.5f, -0.5f}, {0.0f, 0.0f}},
    {{ 0.5f, -0.5f}, {1.0f, 0.0f}},
    {{ 0.5f,  0.5f}, {1.0f, 1.0f}},
    {{-0.5f,  0.5f}, {0.0f, 1.0f}}
};

static const sm_vec2f positions[] = {
    {-0.5f, -0.5f},
    { 0.5f, -0.5f},
    { 0.5f,  0.5f},
    {-0.5f,  0.5f},
};

static const sm_vec2f uvs[] = {
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f},
};

static const sm_vec3f colors[] = {
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 1.0f}
};

static const uint16_t indicies[] = {
    0, 1, 2, 2, 3, 0
};

void DestroyUniformBuffer(VkDevice d, UniformHandles* handles) {
    for (size_t i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(d, handles->bufs[i], NULL);
        vkFreeMemory(d, handles->mem[i], NULL);
    }
}

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
    Texture test2 = {0};

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

    DescriptorType uniformconfig[2] = {SR_DESC_BUF, SR_DESC_SAMPLER};
    DescriptorDetail flags[2] = {{VK_SHADER_STAGE_VERTEX_BIT, 0}, {VK_SHADER_STAGE_FRAGMENT_BIT, 2}};
    result = CreateDescriptorSetConfig(&device, &config, uniformconfig, flags, 2);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);

    AttrConfig vconfig[] = {
        {.rate = VK_VERTEX_INPUT_RATE_VERTEX, .format = VK_FORMAT_R32G32_SFLOAT, .size = sizeof(sm_vec2f)},
        {.rate = VK_VERTEX_INPUT_RATE_VERTEX, .format = VK_FORMAT_R32G32_SFLOAT, .size = sizeof(sm_vec2f)},
    };
    VkVertexInputBindingDescription bind;
    VkVertexInputAttributeDescription attrs[2];
    result = CreateVertAttr(attrs, &bind, vconfig, 2);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);

    VulkanVertexInput vin = {
        .attrs = attrs,
        .binding = bind,
        .size = 2
    };
    result = CreatePipelineConfig(&device, &context, &shader, VulkanVertToConfig(vin), &config);
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

    StaticBuffer vertsBuf = {};
    StaticBuffer indexBuf = {};
    
    result = CreateStaticBuffer(&device, &cmd, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, verts, sizeof(verts), &vertsBuf);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);
    
    result = CreateStaticBuffer(&device, &cmd, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indicies, sizeof(indicies), &indexBuf);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);

    result = CreateImage(&device, &cmd, &test, "resources/textures/texture.jpg");
    if (result != SR_NO_ERROR) {
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);
    }

    result = CreateImage(&device, &cmd, &test2, "resources/textures/duck.jpg");
    if (result != SR_NO_ERROR) {
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);
    }

//    result = CreateUniformBuffer(&uniforms, &test2, &config, &device);
    result = SetImage(&device, test.view, test.sampler, &config, 1, 0);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);

    result = SetImage(&device, test2.view, test2.sampler, &config, 1, 1);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);

    result = SetBuffer(&device, &config, &uniforms, 0);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);

    SpriteHandle s1 = CreateSprite((sm_vec3f){50.0f, 50.0f, 0.0f}, (sm_vec3f){50, 50, 0});
    SpriteHandle s2 = CreateSprite((sm_vec3f){50.0f, 50.0f, 0.0f}, (sm_vec3f){50, 50, 0});


    sm_mat4f* model = GetModel(s2);
    *model = sm_mat4_f32_translate(model, (sm_vec3f){-50.0f, -50.0f, 0.0f});
    *model = sm_mat4_f32_ry(model, SM_PI);
    *model = sm_mat4_f32_translate(model, (sm_vec3f){50.0f, 50.0f, 0.0f});

    RenderState state = {
        .d = &device,
        .context = &context,
        .cmd = &cmd,
        .verts = &vertsBuf,
        .index = &indexBuf,
        .shader = &shader,
        .config = &config,
        .pipeline = &pipeline,
        .pass = &pass,
        .swap = &swapchain,
        .uniforms = &uniforms
    };

    unsigned int frameCounter = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        frameCounter = frameCounter + 1;
        DrawFrame(state, frameCounter % SR_MAX_FRAMES_IN_FLIGHT);
        
        sm_mat4f* model = GetModel(s1);
        *model = sm_mat4_f32_translate(model, (sm_vec3f){-50.0f, -50.0f, 0.0f});
        *model = sm_mat4_f32_ry(model, 0.01);
        *model = sm_mat4_f32_translate(model, (sm_vec3f){50.0f, 50.0f, 0.0f});

        model = GetModel(s2);
        *model = sm_mat4_f32_translate(model, (sm_vec3f){-50.0f, -50.0f, 0.0f});
        *model = sm_mat4_f32_ry(model, 0.01);
        *model = sm_mat4_f32_translate(model, (sm_vec3f){50.0f, 50.0f, 0.0f});
    }

    vkDeviceWaitIdle(device.l);
    DestroyImage(device.l, &test);
    DestroyImage(device.l, &test2);
    DestroyStaticBuffer(device.l, &vertsBuf);
    DestroyStaticBuffer(device.l, &indexBuf);
    ExitProg(window, &context, &device, &swapchain, &shader, &config, &pass, &pipeline, &cmd, &buffer, &uniforms);
    return 0;
}
