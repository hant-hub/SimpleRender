#include "command.h"
#include "common.h"
#include "config.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include "mat4.h"
#include "memory.h"
#include "pipeline.h"
#include "../renderers/multispriteRenderer/multisprite.h"
#include "texture.h"
#include "util.h"
#include "vec2.h"
#include "vec3.h"
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <vulkan/vulkan_core.h>



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

void DestroyUniformBuffer(BufferHandle* handles) {
    VkDevice d = sr_device.l;
    for (size_t i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(d, handles->bufs[i], NULL);
        vkFreeMemory(d, handles->mem[i], NULL);
    }
}

static void ExitProg(SwapChain* swap,
                     VulkanShader* shader, VulkanPipelineConfig* config, RenderPass* r, VulkanPipeline* pipeline, VulkanCommand* cmd,
                        BufferHandle* uniforms) {
    
    DestroyCommand(cmd);
    DestroyPipeline(pipeline);
    DestroyShaderProg(shader);
    DestroyPipelineConfig(config);
    DestroyPass(r);
    DestroyUniformBuffer(uniforms);
    DestroySwapChain(swap);
    DestroyVulkan();
    exit(1);
}


int main() {


    VulkanPipelineConfig config = {0};
    RenderPass pass = {0};
    SwapChain swapchain = {0};
    VulkanShader shader = {0};
    VulkanPipeline pipeline = {0};
    VulkanCommand cmd = {0};
    BufferHandle uniforms = {0};
    Texture test = {0};
    Texture test2 = {0};

    ErrorCode result = CreateVulkan();
    if (result != SR_NO_ERROR)
        ExitProg(&swapchain, &shader, &config, &pass, &pipeline, &cmd, &uniforms);

    result = CreateShaderProg("shaders/standard.vert.spv", "shaders/standard.frag.spv", &shader);
    if (result != SR_NO_ERROR)
        ExitProg(&swapchain, &shader, &config, &pass, &pipeline, &cmd, &uniforms);

    DescriptorType uniformconfig[2] = {SR_DESC_UNIFORM, SR_DESC_SAMPLER};
    DescriptorDetail flags[2] = {{VK_SHADER_STAGE_VERTEX_BIT, 0}, {VK_SHADER_STAGE_FRAGMENT_BIT, 2}};
    result = CreateDescriptorSetConfig(&config, uniformconfig, flags, 2);
    if (result != SR_NO_ERROR)
        ExitProg(&swapchain, &shader, &config, &pass, &pipeline, &cmd, &uniforms);

    AttrConfig vconfig[] = {
        {.rate = VK_VERTEX_INPUT_RATE_VERTEX, .format = VK_FORMAT_R32G32_SFLOAT, .size = sizeof(sm_vec2f)},
        {.rate = VK_VERTEX_INPUT_RATE_VERTEX, .format = VK_FORMAT_R32G32_SFLOAT, .size = sizeof(sm_vec2f)},
    };
    VkVertexInputBindingDescription binds[2];
    VkVertexInputAttributeDescription attrs[2];
    result = MultiCreateVertAttr(attrs, binds, vconfig, 2);
    if (result != SR_NO_ERROR)
        ExitProg(&swapchain, &shader, &config, &pass, &pipeline, &cmd, &uniforms);

    VulkanMultiVertexInput vin = {
        .attrs = attrs,
        .bindings = binds,
        .size = 2
    };
    result = CreatePipelineConfig(&shader, VulkanMultiVertToConfig(vin), &config);
    if (result != SR_NO_ERROR)
        ExitProg(&swapchain, &shader, &config, &pass, &pipeline, &cmd, &uniforms);

    result = CreatePass(&pass);
    if (result != SR_NO_ERROR)
        ExitProg(&swapchain, &shader, &config, &pass, &pipeline, &cmd, &uniforms);

    result = CreateSwapChain(&pass, &swapchain, VK_NULL_HANDLE);
    if (result != SR_NO_ERROR)
        ExitProg(&swapchain, &shader, &config, &pass, &pipeline, &cmd, &uniforms);
    
    result = CreatePipeline(&shader, &config, &pipeline, &pass); 
    if (result != SR_NO_ERROR)
        ExitProg(&swapchain, &shader, &config, &pass, &pipeline, &cmd, &uniforms);

    result = CreateCommand(&cmd);
    if (result != SR_NO_ERROR)
        ExitProg(&swapchain, &shader, &config, &pass, &pipeline, &cmd, &uniforms);

    StaticBuffer posBuf = {};
    StaticBuffer uvBuf = {};
    StaticBuffer indexBuf = {};
    
    result = CreateStaticBuffer(&cmd, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, positions, sizeof(positions), &posBuf);
    if (result != SR_NO_ERROR)
        ExitProg(&swapchain, &shader, &config, &pass, &pipeline, &cmd, &uniforms);
    
    result = CreateStaticBuffer(&cmd, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, uvs, sizeof(uvs), &uvBuf);
    if (result != SR_NO_ERROR)
        ExitProg(&swapchain, &shader, &config, &pass, &pipeline, &cmd, &uniforms);

    result = CreateStaticBuffer(&cmd, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indicies, sizeof(indicies), &indexBuf);
    if (result != SR_NO_ERROR)
        ExitProg(&swapchain, &shader, &config, &pass, &pipeline, &cmd, &uniforms);

    result = CreateImage(&cmd, &test, "resources/textures/texture.jpg");
    if (result != SR_NO_ERROR) {
        ExitProg(&swapchain, &shader, &config, &pass, &pipeline, &cmd, &uniforms);
    }

    result = CreateImage(&cmd, &test2, "resources/textures/duck.jpg");
    if (result != SR_NO_ERROR) {
        ExitProg(&swapchain, &shader, &config, &pass, &pipeline, &cmd, &uniforms);
    }

//    result = CreateUniformBuffer(&uniforms, &test2, &config, &device);
    result = SetImage(test.view, test.sampler, &config, 1, 0);
    if (result != SR_NO_ERROR)
        ExitProg(&swapchain, &shader, &config, &pass, &pipeline, &cmd, &uniforms);

    result = SetImage(test2.view, test2.sampler, &config, 1, 1);
    if (result != SR_NO_ERROR)
        ExitProg(&swapchain, &shader, &config, &pass, &pipeline, &cmd, &uniforms);

    result = SetBuffer(&config, &uniforms, 0);
    if (result != SR_NO_ERROR)
        ExitProg(&swapchain, &shader, &config, &pass, &pipeline, &cmd, &uniforms);

    SpriteHandle s1 = MultiCreateSprite((sm_vec3f){50.0f, 50.0f, 0.0f}, (sm_vec3f){50, 50, 0});
    SpriteHandle s2 = MultiCreateSprite((sm_vec3f){50.0f, 50.0f, 0.0f}, (sm_vec3f){50, 50, 0});


    sm_mat4f* model = MultiGetModel(s2);
    *model = sm_mat4_f32_translate(model, (sm_vec3f){-50.0f, -50.0f, 0.0f});
    *model = sm_mat4_f32_ry(model, SM_PI);
    *model = sm_mat4_f32_translate(model, (sm_vec3f){50.0f, 50.0f, 0.0f});

    RenderState state = {
        .cmd = &cmd,
        .pos = &posBuf,
        .uvs = &uvBuf,
        .index = &indexBuf,
        .shader = &shader,
        .config = &config,
        .pipeline = &pipeline,
        .pass = &pass,
        .swap = &swapchain,
        .uniforms = &uniforms
    };

    unsigned int frameCounter = 0;
    while (!glfwWindowShouldClose(sr_context.w)) {
        glfwPollEvents();
        frameCounter = (frameCounter + 1) % SR_MAX_FRAMES_IN_FLIGHT;
        DrawFrameMultiSprite(state, frameCounter);
        
        sm_mat4f* model = MultiGetModel(s1);
        *model = sm_mat4_f32_translate(model, (sm_vec3f){-50.0f, -50.0f, 0.0f});
        *model = sm_mat4_f32_ry(model, 0.01);
        *model = sm_mat4_f32_translate(model, (sm_vec3f){50.0f, 50.0f, 0.0f});

        model = MultiGetModel(s2);
        *model = sm_mat4_f32_translate(model, (sm_vec3f){-50.0f, -50.0f, 0.0f});
        *model = sm_mat4_f32_ry(model, 0.01);
        *model = sm_mat4_f32_translate(model, (sm_vec3f){50.0f, 50.0f, 0.0f});
    }

    vkDeviceWaitIdle(sr_device.l);
    DestroyImage(&test);
    DestroyImage(&test2);
    DestroyStaticBuffer(&posBuf);
    DestroyStaticBuffer(&uvBuf);
    DestroyStaticBuffer(&indexBuf);
    ExitProg(&swapchain, &shader, &config, &pass, &pipeline, &cmd, &uniforms);
    return 0;
}
