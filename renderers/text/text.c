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
} Vertex;

typedef struct {
    sm_mat4f proj;
    sm_mat4f view;
} Uniform;

typedef struct {
    uint glyph;
    f32 rotation;
    f32 scale;
    //4 bytes of additional padding
} __attribute__ ((aligned(sizeof(sm_vec4f)))) Character;


ErrorCode SetArea(TextRenderer* r, sm_vec2f area) {
    r->textarea = area;
    return SR_NO_ERROR;
}

ErrorCode TextInit(TextRenderer* r, const char* font, u32 size, RenderPass* p, u32 subpass) {

    PASS_CALL(CreateShaderProg("shaders/text/text.vert.spv", "shaders/text/text.frag.spv", &r->shader));

    DescriptorDetail descriptorConfigs[] = {
        {SR_DESC_UNIFORM, VK_SHADER_STAGE_VERTEX_BIT, 0},
        {SR_DESC_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1},
        {SR_DESC_UNIFORM, VK_SHADER_STAGE_FRAGMENT_BIT, 0},
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
    PASS_CALL(CreatePipelineConfig(&r->shader, VulkanVertToConfig(vin), &r->config, FALSE));

    PASS_CALL(CreatePipeline(&r->shader, &r->config, &r->pipeline, p, subpass)); 
    
    LoadFont(font, size, &r->fdata);

    PASS_CALL(CreateDynamicBuffer(MAX_CHARS * 6 * sizeof(uint16_t), &r->indicies, VK_BUFFER_USAGE_INDEX_BUFFER_BIT));
    PASS_CALL(CreateDynamicBuffer(MAX_CHARS * 4 * sizeof(sm_vec2f), &r->verts, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT));
    PASS_CALL(CreateDynamicBuffer(sizeof(Uniform), &r->vertuniforms, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));
    PASS_CALL(CreateDynamicBuffer(sizeof(sm_vec3f), &r->fraguniforms, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));

    //todo, fix SetBuffer to be non sprite specific
    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        PASS_CALL(SetBuffer(&r->config, SR_DESC_UNIFORM, (Buffer*)&r->vertuniforms, 0, i));
        PASS_CALL(SetBuffer(&r->config, SR_DESC_UNIFORM, (Buffer*)&r->fraguniforms, 2, i));
        PASS_CALL(SetImage(r->fdata.atlas.image.view, r->fdata.atlas.sampler, &r->config, 1, 0, i));
    }

    ((sm_vec3f*)r->fraguniforms.buffer)[0] = (sm_vec3f){1.0f, 1.0f, 1.0f};
    return SR_NO_ERROR;
}

void TextDestroy(TextRenderer* r) {
    vkDeviceWaitIdle(sr_device.l);

    DestroyTexture(&r->fdata.atlas);

    DestroyStaticBuffer(&r->font);
    DestroyDynamicBuffer(&r->indicies);
    DestroyDynamicBuffer(&r->verts);
    DestroyDynamicBuffer(&r->vertuniforms);
    DestroyDynamicBuffer(&r->fraguniforms);
    DestroyPipeline(&r->pipeline);
    DestroyShaderProg(&r->shader);
    DestroyPipelineConfig(&r->config);
}

ErrorCode ClearText(TextRenderer* r) {
    r->chars = 0;
    return SR_NO_ERROR;
}

ErrorCode SetColor(TextRenderer* r, sm_vec3f color) {
    ((sm_vec3f*)r->fraguniforms.buffer)[0] = color;
    return SR_NO_ERROR;
}

ErrorCode AppendText(TextRenderer* r, const char* text, u32 textLen, sm_vec2f pos, float scale) {
    float cadvance = 0;
    float cdrop = 0;
    int nonrendered = 0;
    Vertex* vertex = r->verts.buffer;
    uint16_t* in = r->indicies.buffer;

    for (int i = 0; i < textLen; i++) {
        sm_vec2i size = r->fdata.size[text[i]];
        //sm_vec2i pos = r->fdata.pos[text[i]]; For doing UV lookups, not needed yet
        sm_vec2i offset = r->fdata.offset[text[i]];
        int advance = r->fdata.advance[text[i]]; 
        if (text[i] == '\n') {
            cdrop += (size.y * scale) * 1.50;
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

        int idx = i + r->chars;
        vertex[idx * 4 + 0] = (Vertex) {
            .pos = {x, y},
            .uv = {u , v}
        };
        vertex[idx * 4 + 1] = (Vertex) {
            .pos = {x + ((float)(size.x) * scale), y},
            .uv = {u + texsize.x, v}
        };
        vertex[idx * 4 + 2] = (Vertex) {
            .pos = {x + (float)size.x * scale, y + (float)size.y * scale},
            .uv = {u + texsize.x, v + texsize.y}
        };
        vertex[idx * 4 + 3] = (Vertex) {
            .pos = {x, y + (float)size.y * scale},
            .uv = {u, v + texsize.y}
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
    return SR_NO_ERROR;
}

ErrorCode ReplaceText(TextRenderer* r, const char* text, u32 start, u32 end, float scale);

ErrorCode TextGetSubpass(SubPass* s, Attachment* a, u32 start) {
    *s = (SubPass) {
        .numAttachments = 0,
        .colorAttachment = 0,
        .depthAttachment = -1,
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

    float aspect = ((float)WIDTH)/((float)HEIGHT);
    proj = sm_mat4_f32_ortho(0.5f, 2.0f, -aspect, aspect, -1.0f, 1.0f);
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
