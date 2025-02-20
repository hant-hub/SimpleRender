#include "text.h"

#include "config.h"
#include "error.h"
#include "frame.h"
#include "init.h"
#include "mat4.h"
#include "memory.h"
#include "texture.h"
#include "util.h"
#include "vec2.h"
#include "vec4.h"
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <vulkan/vulkan_core.h>


typedef struct {
    sm_vec2f pos;
    sm_vec2f uv;
    sm_vec4f color;
    float layer;
} Vertex;

typedef struct {
    sm_mat4f proj;
    sm_mat4f view;
} Uniform;


ErrorCode SetArea(TextRenderer* r, sm_vec2f area) {
    r->textarea = area;
    return SR_NO_ERROR;
}

ErrorCode TextInit(TextRenderer* r, Font* f, u32 size, RenderPass* p, u32 subpass) {

    PASS_CALL(CreateShaderProg("shaders/text/text.vert.spv", "shaders/text/text.frag.spv", &r->shader));

    DescriptorDetail descriptorConfigs[] = {
        {SR_DESC_UNIFORM, VK_SHADER_STAGE_VERTEX_BIT, 0},
        {SR_DESC_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1},
    };
    PASS_CALL(CreateDescriptorSetConfig(&r->config, descriptorConfigs, ARRAY_SIZE(descriptorConfigs)));
    
    AttrConfig vconfig[] = {
        {.rate = VK_VERTEX_INPUT_RATE_VERTEX, .format = VK_FORMAT_R32G32_SFLOAT, .size = sizeof(sm_vec2f)},
        {.rate = VK_VERTEX_INPUT_RATE_VERTEX, .format = VK_FORMAT_R32G32_SFLOAT, .size = sizeof(sm_vec2f)},
        {.rate = VK_VERTEX_INPUT_RATE_VERTEX, .format = VK_FORMAT_R32G32B32A32_SFLOAT, .size = sizeof(sm_vec4f)},
        {.rate = VK_VERTEX_INPUT_RATE_VERTEX, .format = VK_FORMAT_R32_SFLOAT, .size = sizeof(float)},
    };
    VkVertexInputBindingDescription binds[1];
    VkVertexInputAttributeDescription attrs[4];
    PASS_CALL(CreateVertAttr(attrs, binds, vconfig, 4));


    VulkanVertexInput vin = {
        .attrs = attrs,
        .binding = binds[0],
        .size = 4
    };
    PASS_CALL(CreatePipelineConfig(&r->shader, VulkanVertToConfig(vin), &r->config, TRUE));

    PASS_CALL(CreatePipeline(&r->shader, &r->config, &r->pipeline, p, subpass)); 
    
    r->fdata = f;

    PASS_CALL(CreateDynamicBuffer(MAX_CHARS * 6 * sizeof(uint16_t), &r->indicies, VK_BUFFER_USAGE_INDEX_BUFFER_BIT));
    PASS_CALL(CreateDynamicBuffer(MAX_CHARS * 4 * sizeof(Vertex), &r->verts, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT));
    PASS_CALL(CreateDynamicBuffer(sizeof(Uniform), &r->vertuniforms, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));

    //todo, fix SetBuffer to be non sprite specific
    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        PASS_CALL(SetBuffer(&r->config, SR_DESC_UNIFORM, (Buffer*)&r->vertuniforms, 0, i));
        PASS_CALL(SetImage(r->fdata->atlas.image.view, r->fdata->atlas.sampler, &r->config, 1, 0, i));
    }

    r->currentTextColor = (sm_vec4f){
        1.0, 1.0, 1.0, 1.0
    };

    return SR_NO_ERROR;
}

void TextDestroy(TextRenderer* r) {
    vkDeviceWaitIdle(sr_device.l);

    DestroyDynamicBuffer(&r->indicies);
    DestroyDynamicBuffer(&r->verts);
    DestroyDynamicBuffer(&r->vertuniforms);
    DestroyPipeline(&r->pipeline);
    DestroyShaderProg(&r->shader);
    DestroyPipelineConfig(&r->config);
}

ErrorCode ClearText(TextRenderer* r) {
    r->chars = 0;
    return SR_NO_ERROR;
}

ErrorCode SetColor(TextRenderer* r, sm_vec3f color) {
    r->currentTextColor = (sm_vec4f){
        color.x, color.y, color.z, 1.0
    };
    return SR_NO_ERROR;
}

ErrorCode AppendText(TextRenderer* r, const char* text, u32 textLen, sm_vec2f pos, float layer, float scale) {
    float cadvance = 0;
    float cdrop = 0;
    int nonrendered = 0;
    Vertex* vertex = r->verts.buffer;
    uint16_t* in = r->indicies.buffer;

    int start = r->chars;
    int newlines = 0;

    for (int i = 0; i < textLen; i++) {
        sm_vec2i size = r->fdata->size[text[i]];
        //sm_vec2i pos = r->fdata.pos[text[i]]; For doing UV lookups, not needed yet
        sm_vec2i offset = r->fdata->offset[text[i]];
        int advance = r->fdata->advance[text[i]]; 
        if (text[i] == '\n') {
            cdrop += (size.y * scale) * 1.50;
            cadvance = 0;
            newlines++;
            continue;
        }

        float x = pos.x + cadvance + (offset.x * scale);
        float y = pos.y + cdrop - (offset.y) * scale;

        float u = ((float)r->fdata->pos[text[i]].x) / r->fdata->texsize.x;
        float v = ((float)r->fdata->pos[text[i]].y) / r->fdata->texsize.y;

        sm_vec2f texsize = (sm_vec2f){
            ((float)size.x) / r->fdata->texsize.x,
            ((float)size.y) / r->fdata->texsize.y
        };

        int idx = i + start - newlines;
        vertex[idx * 4 + 0] = (Vertex) {
            .pos = {x, y},
            .uv = {u , v},
            .color = r->currentTextColor,
            .layer = layer
        };
        vertex[idx * 4 + 1] = (Vertex) {
            .pos = {x + ((float)(size.x) * scale), y},
            .uv = {u + texsize.x, v},
            .color = r->currentTextColor,
            .layer = layer
        };
        vertex[idx * 4 + 2] = (Vertex) {
            .pos = {x + (float)size.x * scale, y + (float)size.y * scale},
            .uv = {u + texsize.x, v + texsize.y},
            .color = r->currentTextColor,
            .layer = layer
        };
        vertex[idx * 4 + 3] = (Vertex) {
            .pos = {x, y + (float)size.y * scale},
            .uv = {u, v + texsize.y},
            .color = r->currentTextColor,
            .layer = layer
        };

        in[idx*6 + 0] = idx * 4 + 0;
        in[idx*6 + 1] = idx * 4 + 1;
        in[idx*6 + 2] = idx * 4 + 2;
        in[idx*6 + 3] = idx * 4 + 2;
        in[idx*6 + 4] = idx * 4 + 3;
        in[idx*6 + 5] = idx * 4 + 0;
        cadvance += (float)advance * scale;
        r->chars++;
    }

    r->appendPos = (sm_vec2f) {
        pos.x + cadvance, pos.y + cdrop
    };


    return SR_NO_ERROR;
}

ErrorCode ReplaceText(TextRenderer* r, const char* text, u32 start, u32 end, float scale);
sm_vec2f GetTextPos(TextRenderer* r) {
    return r->appendPos;
}

ErrorCode TextGetSubpass(SubPass* s, Attachment* a, u32 start) {
    s[start] = (SubPass) {
        .numAttachments = SR_TEXT_NUM_ATTACHMENTS,
        .colorAttachment = 0,
        .depthAttachment = 0,
        .firstAttachment = 0,
    };
    return SR_NO_ERROR;
}

void TextDrawFrame(TextRenderer* r, PresentInfo* p, u32 frame) {
    VulkanDevice* device = &sr_device;
    VulkanContext* context = &sr_context; 
    VulkanCommand* cmd = &sr_context.cmd;
    VulkanShader* shader = &r->shader;
    VulkanPipelineConfig* config = &r->config;
    VulkanPipeline* pipe = &r->pipeline;

    sm_mat4f view = SM_MAT4_IDENTITY;
    sm_mat4f proj = SM_MAT4_IDENTITY;

    float aspect = ((float)fWIDTH)/((float)fHEIGHT);
    proj = sm_mat4_f32_ortho(0.5f, 100 * 1.0f, -aspect, aspect, -1.0f, 1.0f);
    view = sm_mat4_f32_scale(&view, (sm_vec4f){1/r->textarea.x, 1/r->textarea.y, 1.0f, 1.0f});

    Uniform u = (Uniform){
        .proj = proj,
        .view = view
    };
    memcpy(r->vertuniforms.buffer, &u, sizeof(Uniform));
    VkCommandBuffer cmdBuf = cmd->buffer[frame];

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

    vkCmdDrawIndexed(cmdBuf, r->chars * 6, 1, 0, 0, 0);
}
