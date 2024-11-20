#include "text.h"

#include "config.h"
#include "error.h"
#include "frame.h"
#include "init.h"
#include "log.h"
#include "mat4.h"
#include "memory.h"
#include "texture.h"
#include "util.h"
#include "vec2.h"
#include "vec4.h"
#include <stdint.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

typedef struct {
    sm_vec2f pos;
    sm_vec2f uv;
} Vertex;

typedef struct {
    uint glyph;
    f32 rotation;
    f32 scale;
    //4 bytes of additional padding
} __attribute__ ((aligned(sizeof(sm_vec4f)))) Character;



ErrorCode TextInit(TextRenderer* r) {

    PASS_CALL(CreateShaderProg("shaders/text/text.vert.spv", "shaders/text/text.frag.spv", &r->shader));

    DescriptorDetail descriptorConfigs[] = {
        {SR_DESC_UNIFORM, VK_SHADER_STAGE_VERTEX_BIT, 0},
        {SR_DESC_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1}
    };
    PASS_CALL(CreateDescriptorSetConfig(&r->config, descriptorConfigs, ARRAY_SIZE(descriptorConfigs)));
    
    AttrConfig vconfig[] = {
        {.rate = VK_VERTEX_INPUT_RATE_VERTEX, .format = VK_FORMAT_R32G32_SFLOAT, .size = sizeof(sm_vec2f)},
        {.rate = VK_VERTEX_INPUT_RATE_VERTEX, .format = VK_FORMAT_R32G32_SFLOAT, .size = sizeof(sm_vec2f)}
    };
    VkVertexInputBindingDescription binds[1];
    VkVertexInputAttributeDescription attrs[2];
    PASS_CALL(CreateVertAttr(attrs, binds, vconfig, 2));


    VulkanVertexInput vin = {
        .attrs = attrs,
        .binding = binds[0],
        .size = 2
    };
    PASS_CALL(CreatePipelineConfig(&r->shader, VulkanVertToConfig(vin), &r->config));

    PASS_CALL(CreatePass(&r->pass, NULL, NULL, 0));
//    PASS_CALL(CreateSwapChain(&r->pass, &r->swap, VK_NULL_HANDLE));
    PASS_CALL(CreatePipeline(&r->shader, &r->config, &r->pipeline, &r->pass)); 
    
    LoadFont(&r->fdata);

    PASS_CALL(CreateDynamicBuffer(MAX_CHARS * 4 * sizeof(uint16_t), &r->indicies, VK_BUFFER_USAGE_INDEX_BUFFER_BIT));
    PASS_CALL(CreateDynamicBuffer(MAX_CHARS * 4 * sizeof(sm_vec2f), &r->verts, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT));
    PASS_CALL(CreateDynamicBuffer(sizeof(sm_mat4f), &r->uniforms, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));

    //todo, fix SetBuffer to be non sprite specific
    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        PASS_CALL(SetBuffer(&r->config, SR_DESC_UNIFORM, (Buffer*)&r->uniforms, 0, i));
        PASS_CALL(SetImage(r->fdata.atlas.image.view, r->fdata.atlas.sampler, &r->config, 1, 0, i));
    }

    return SR_NO_ERROR;
}

void TextDestroy(TextRenderer* r) {
    vkDeviceWaitIdle(sr_device.l);

    DestroyTexture(&r->fdata.atlas);

    DestroyStaticBuffer(&r->font);
    DestroyDynamicBuffer(&r->indicies);
    DestroyDynamicBuffer(&r->verts);
    DestroyDynamicBuffer(&r->uniforms);
    DestroyPipeline(&r->pipeline);
    DestroyShaderProg(&r->shader);
    DestroyPipelineConfig(&r->config);
    DestroyPass(&r->pass);
    VkDevice d = sr_device.l;
    DestroySwapChain(&r->swap, &r->pass);
}

ErrorCode SetText(TextRenderer* r, const char* text, u32 textlen, sm_vec2f pos, float scale) {
    static const Vertex verts[] = {
        {{ 1, 1}, 0},
        {{ 2, 1}, 1},
        {{ 2, 2}, 1},
        {{ 1, 2}, 0},
    };

    static const uint16_t indicies[] = {
        0, 1, 2, 2, 3, 0
    };

    float cadvance = 0;
    float cdrop = 0;
    int nonrendered = 0;
    Vertex* vertex = r->verts.buffer;
    uint16_t* in = r->indicies.buffer;
    scale *= 0.0001 * HEIGHT;
    for (int i = 0; i < textlen; i++) {

        sm_vec2i size = r->fdata.size[text[i]];
        //sm_vec2i pos = r->fdata.pos[text[i]]; For doing UV lookups, not needed yet
        sm_vec2i offset = r->fdata.offset[text[i]];
        int advance = r->fdata.advance[text[i]]; 
        if (text[i] == '\n') {
            cdrop += (size.y * scale) + 3;
            cadvance = 0;
            continue;
        }

        float x = pos.x + cadvance + (offset.x * scale);
        float y = pos.y + cdrop - (offset.y) * scale;

        float u = ((float)r->fdata.pos[text[i]].x) / r->fdata.texsize.x;
        float v = ((float)r->fdata.pos[text[i]].y) / r->fdata.texsize.y;

        sm_vec2f texsize = (sm_vec2f){
            ((float)size.x) / r->fdata.texsize.x,
            ((float)size.y) / r->fdata.texsize.y
        };

        vertex[i * 4 + 0] = (Vertex) {
            .pos = {x, y},
            .uv = {u , v}
        };
        vertex[i * 4 + 1] = (Vertex) {
            .pos = {x + ((float)(size.x) * scale), y},
            .uv = {u + texsize.x, v}
        };
        vertex[i * 4 + 2] = (Vertex) {
            .pos = {x + (float)size.x * scale, y + (float)size.y * scale},
            .uv = {u + texsize.x, v + texsize.y}
        };
        vertex[i * 4 + 3] = (Vertex) {
            .pos = {x, y + (float)size.y * scale},
            .uv = {u, v + texsize.y}
        };

        in[i*6 + 0] = i * 4 + 0;
        in[i*6 + 1] = i * 4 + 1;
        in[i*6 + 2] = i * 4 + 2;
        in[i*6 + 3] = i * 4 + 2;
        in[i*6 + 4] = i * 4 + 3;
        in[i*6 + 5] = i * 4 + 0;
        cadvance += (float)advance * scale;
    }
    r->chars = (textlen - nonrendered) * 6;

    return SR_NO_ERROR;
}

void TextDrawFrame(TextRenderer* r, PresentInfo* p, u32 frame) {
    VulkanDevice* device = &sr_device;
    VulkanContext* context = &sr_context; 
    VulkanCommand* cmd = &sr_context.cmd;
    VulkanShader* shader = &r->shader;
    VulkanPipelineConfig* config = &r->config;
    VulkanPipeline* pipe = &r->pipeline;
    RenderPass* pass = &r->pass;

    sm_mat4f view = SM_MAT4_IDENTITY;
    sm_mat4f proj = SM_MAT4_IDENTITY;
    float aspect = ((float)WIDTH)/((float)HEIGHT);
    proj = sm_mat4_f32_ortho(0.0f,  1.0f, 0.0f, 100.0f * aspect, 0.0f, 100.0f);

    memcpy(r->uniforms.buffer, &proj, sizeof(sm_mat4f));

    vkResetFences(sr_device.l, 1, &p->inFlight[frame]);
    vkResetCommandBuffer(cmd->buffer[frame], 0);
    VkCommandBuffer cmdBuf = cmd->buffer[frame];
    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    if (vkBeginCommandBuffer(cmdBuf, &beginInfo) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Begin Command Buffer");
        return;
    }

    VkRenderPassBeginInfo renderInfo = {0};
    renderInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderInfo.renderPass = pass->pass;
    renderInfo.framebuffer = p->swapchain.buffers[p->imageIndex];
    renderInfo.renderArea.offset = (VkOffset2D){0, 0};
    renderInfo.renderArea.extent = p->swapchain.extent;

    VkClearValue clearcolors[2] = {0}; 
    clearcolors[0].color = (VkClearColorValue){0.0f, 0.0f, 0.0f, 1.0f};
    renderInfo.clearValueCount = 1;
    renderInfo.pClearValues = clearcolors;

    vkCmdBeginRenderPass(cmdBuf, &renderInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipe->pipeline);

    VkBuffer bufs[] = {r->verts.vhandle};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmdBuf, 0, 1, bufs, offsets);
    vkCmdBindIndexBuffer(cmdBuf, r->indicies.vhandle, 0, VK_INDEX_TYPE_UINT16); 

    pipe->view.x = 0.0f;
    pipe->view.y = 0.0f;
    pipe->view.width = p->swapchain.extent.width;
    pipe->view.height = p->swapchain.extent.height;
    pipe->view.minDepth = 0.0f;
    pipe->view.maxDepth = 1.0f;
    vkCmdSetViewport(cmdBuf, 0, 1, &pipe->view);

    pipe->scissor.offset = (VkOffset2D){0, 0};
    pipe->scissor.extent = p->swapchain.extent;
    vkCmdSetScissor(cmdBuf, 0, 1, &pipe->scissor);

    vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, config->layout, 0, 1, &config->descrip.descriptorSet[frame], 0, NULL);

    vkCmdDrawIndexed(cmdBuf, r->chars, 1, 0, 0, 0);
    vkCmdEndRenderPass(cmdBuf);

    if (vkEndCommandBuffer(cmdBuf) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to End Command Buffer");
        return;
    }

    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {p->imageAvalible[frame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd->buffer[frame];

    VkSemaphore signalSemaphores[] = {p->renderFinished[frame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(device->graph, 1, &submitInfo, p->inFlight[frame]) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Submit Queue");
        return;
    }
}
