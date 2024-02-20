#include "command.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include "pipeline.h"
#include "swap.h"
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <vulkan/vulkan_core.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;


static void ExitProg(GLFWwindow* window, VulkanContext* context, VulkanDevice* device, SwapChain* swap,
                     VulkanShader* shader, VulkanPipelineConfig* config, VulkanPipeline* pipeline, VulkanCommand* cmd) {
    
    DestroyCommand(cmd, device);
    DestroyPipeline(device->l, pipeline);
    DestroyShaderProg(device->l, shader);
    DestroyPipelineConfig(device->l, config);
    DestroySwapChain(device->l, swap);
    DestroyDevice(device);
    DestroyContext(context);
    glfwDestroyWindow(window);
    glfwTerminate();
}

static void DrawFrame(VulkanDevice* device, VulkanCommand* cmd, SwapChain* swapchain, VulkanPipeline* pipe, unsigned int frame) {
    vkWaitForFences(device->l, 1, &cmd->inFlight[frame], VK_TRUE, UINT64_MAX);
    vkResetFences(device->l, 1, &cmd->inFlight[frame]);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(device->l, swapchain->swapChain, UINT64_MAX, cmd->imageAvalible[frame], NULL, &imageIndex);

    vkResetCommandBuffer(cmd->buffer[frame], 0);
    RecordCommandBuffer(swapchain, pipe, &cmd->buffer[frame], imageIndex);

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

    vkQueuePresentKHR(device->present, &presentInfo);
}

int main() {

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "SimpleRender", NULL, NULL);

    VulkanContext context = {0};
    VulkanDevice device = {0};
    SwapChain swapchain = {0};
    VulkanShader shader = {0};
    VulkanPipelineConfig config = {0};
    VulkanPipeline pipeline = {0};
    VulkanCommand cmd = {0};

    ErrorCode result = CreateInstance(&context);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd);
    context.w = window;

    result = CreateSurface(&context, window);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd);

    result = CreateDevices(&device, &context);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd);

    result = CreateSwapChain(&device, &context, &swapchain, VK_NULL_HANDLE);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd);
    
    result = CreateShaderProg(device.l, "shaders/standard.vert.spv", "shaders/standard.frag.spv", &shader);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd);

    result = CreatePipelineConfig(&device, &context, swapchain.format.format, &shader, &config);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd);

    result = CreateFrameBuffers(&device, &swapchain, &config);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd);

    
    result = CreatePipeline(&device, &context, &shader, &config, &pipeline);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd);

    result = CreateCommand(&cmd, &context, &device);
    if (result != SR_NO_ERROR)
        ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd);


    unsigned int frameCounter = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        DrawFrame(&device, &cmd, &swapchain, &pipeline, frameCounter % SR_MAX_FRAMES_IN_FLIGHT);
        frameCounter = frameCounter + 1;
    }

    vkDeviceWaitIdle(device.l);
    ExitProg(window, &context, &device, &swapchain, &shader, &config, &pipeline, &cmd);
    return 0;
}
