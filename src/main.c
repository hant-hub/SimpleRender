#include "command.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include "mat4.h"
#include "pipeline.h"
#include "swap.h"
#include "util.h"
#include "vertex.h"
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

static uint32_t WIDTH = 800;
static uint32_t HEIGHT = 600;

static bool frameBufferResized;

static const Vertex verticies[] = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

static const uint16_t indicies[] = {
    0, 1, 2, 2, 3, 0
};

static void ExitProg(GLFWwindow* window, VulkanContext* context, VulkanDevice* device, SwapChain* swap,
                     VulkanShader* shader, VulkanPipelineConfig* config, VulkanPipeline* pipeline, VulkanCommand* cmd, GeometryBuffer* buffer,
                     UniformHandles* uniforms) {
    
    DestroyBuffer(device->l, buffer);
    DestroyCommand(cmd, device);
    DestroyPipeline(device->l, pipeline);
    DestroyShaderProg(device->l, shader);
    DestroyPipelineConfig(device->l, config);
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

static void DrawFrame(VulkanDevice* device, VulkanCommand* cmd, GeometryBuffer* buffer, VulkanContext* context, VulkanShader* s,
                      VulkanPipelineConfig* config, SwapChain* swapchain, VulkanPipeline* pipe, UniformHandles* uniforms, unsigned int frame) {

    vkWaitForFences(device->l, 1, &cmd->inFlight[frame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device->l, swapchain->swapChain, UINT64_MAX, cmd->imageAvalible[frame], NULL, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {

        vkDeviceWaitIdle(device->l);
        DestroySwapChain(device->l, swapchain);

        ErrorCode code = CreateSwapChain(device, context, swapchain, NULL);
        if (code != SR_NO_ERROR) return;
        code = CreateFrameBuffers(device, swapchain, config);
        if (code != SR_NO_ERROR) return;


    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        SR_LOG_ERR("Bad things are happening");
    }

    static sm_mat4f model = {
        {1.0f, 0, 0, 0},
        {0, 1.0f, 0, 0},
        {0, 0, 1.0f, 0},
        {0, 0, 0, 1.0f}
    };


    sm_mat4f scaler = {
        {1.0f, 0, 0, 0},
        {0, 1.0f, 0, 0},
        {0, 0, 1.0f, 0},
        {0, 0, 0, 1.0f}
    };

    sm_vec3f mov = (sm_vec3f) {0.01f, 0, 0};


    UniformObj uniformData = {0};
    sm_mat4_f32_identity(&uniformData.model);
    sm_mat4_f32_identity(&uniformData.view);
    sm_mat4_f32_identity(&uniformData.proj);

    sm_mat4_f32_comp(&model, &model, &scaler);
    sm_mat4_f32_translate(&model, mov);
    sm_mat4_f32_comp(&uniformData.model, &uniformData.model, &model);


    memcpy(uniforms->objs[frame], &uniformData, sizeof(UniformObj));

    vkResetFences(device->l, 1, &cmd->inFlight[frame]);
    vkResetCommandBuffer(cmd->buffer[frame], 0);
    RecordCommandBuffer(swapchain, pipe, config, &cmd->buffer[frame], buffer, imageIndex, frame);

    //update uniforms



    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {cmd->imageAvalible[frame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd->buffer[frame];

    VkSemaphore signalSemaphores[] = {cmd->renderFinished[frame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(device->graph, 1, &submitInfo, cmd->inFlight[frame]) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Submit Queue");
        return;
    }

    VkPresentInfoKHR presentInfo = {0};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapchain->swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(device->present, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR ||
        result == VK_SUBOPTIMAL_KHR ||
        frameBufferResized) {

        frameBufferResized = FALSE;
        vkDeviceWaitIdle(device->l);
        DestroySwapChain(device->l, swapchain);

        ErrorCode code = CreateSwapChain(device, context, swapchain, NULL);
        if (code != SR_NO_ERROR) return;
        code = CreateFrameBuffers(device, swapchain, config);
        if (code != SR_NO_ERROR) return;

    } else if (result != VK_SUCCESS) {
        SR_LOG_ERR("Bad things are happening");
    }
}

int main() {

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "SimpleRender", NULL, NULL);
    glfwSetFramebufferSizeCallback(window, ResizeCallback);

    VulkanContext context = {0};
    VulkanDevice device = {0};
    SwapChain swapchain = {0};
    VulkanShader shader = {0};
    VulkanPipelineConfig config = {0};
    VulkanPipeline pipeline = {0};
    VulkanCommand cmd = {0};
    GeometryBuffer buffer = {0};
    UniformHandles uniforms = {0};

    ErrorCode result = CreateInstance(&context);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd, &buffer, &uniforms);
    context.w = window;

    result = CreateSurface(&context, window);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd, &buffer, &uniforms);

    result = CreateDevices(&device, &context);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd, &buffer, &uniforms);

    result = CreateSwapChain(&device, &context, &swapchain, VK_NULL_HANDLE);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd, &buffer, &uniforms);
    
    result = CreateShaderProg(device.l, "shaders/standard.vert.spv", "shaders/standard.frag.spv", &shader);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd, &buffer, &uniforms);

    result = CreatePipelineConfig(&device, &context, swapchain.format.format, &shader, &config);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd, &buffer, &uniforms);

    result = CreateFrameBuffers(&device, &swapchain, &config);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd, &buffer, &uniforms);
    
    result = CreatePipeline(&device, &context, &shader, &config, &pipeline);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd, &buffer, &uniforms);

    result = CreateCommand(&cmd, &context, &device);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd, &buffer, &uniforms);

    result = CreateStaticGeometry(&buffer, verticies, indicies, sizeof(verticies), sizeof(indicies), &device, &cmd);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd, &buffer, &uniforms);

    result = CreateUniformBuffer(&uniforms, &config, &device);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd, &buffer, &uniforms);

    unsigned int frameCounter = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        frameCounter = frameCounter + 1;
        DrawFrame(&device, &cmd, &buffer, &context, &shader, &config, &swapchain, &pipeline, &uniforms, frameCounter % SR_MAX_FRAMES_IN_FLIGHT);
    }

    vkDeviceWaitIdle(device.l);
    ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd, &buffer, &uniforms);
    return 0;
}
