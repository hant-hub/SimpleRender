#include "fractal.h"
#include "config.h"
#include "error.h"
#include "frame.h"
#include "init.h"
#include "mat4.h"
#include "memory.h"
#include "pipeline.h"
#include "util.h"
#include "vec2.h"
#include <vulkan/vulkan_core.h>

typedef struct {
    sm_vec2f pos;
    sm_vec2f uv;
} Vertex;


static const Vertex verts[] = {
    {{-1.0f, -1.0f}, {0.0f, 0.0f}},
    {{ 1.0f, -1.0f}, {1.0f, 0.0f}},
    {{ 1.0f,  1.0f}, {1.0f, 1.0f}},
    {{-1.0f,  1.0f}, {0.0f, 1.0f}},
};

static const uint16_t indicies[] = {
    0, 1, 2, 2, 3, 0
};


ErrorCode FractalInit(FractalRenderer* r) {

    PASS_CALL(CreateShaderProg("shaders/fractal/fractal.vert", "shaders/fractal/fractal.frag", &r->shader));

    AttrConfig vconfig[] = {
        {.rate = VK_VERTEX_INPUT_RATE_VERTEX, .format = VK_FORMAT_R32G32_SFLOAT, .size = sizeof(sm_vec2f)},
        {.rate = VK_VERTEX_INPUT_RATE_VERTEX, .format = VK_FORMAT_R32G32_SFLOAT, .size = sizeof(sm_vec2f)},
    };
    VkVertexInputBindingDescription binds[1];
    VkVertexInputAttributeDescription attrs[2];
    PASS_CALL(CreateVertAttr(attrs, binds, vconfig, 2));

    DescriptorDetail layout[] = {
        {SR_DESC_UNIFORM, 0}
    };
    PASS_CALL(CreateDescriptorSetConfig(&r->config, layout, ARRAY_SIZE(layout)));

    VulkanVertexInput vin = {
        .attrs = attrs,
        .binding = binds[0],
        .size = 2
    };
    PASS_CALL(CreatePipelineConfig(&r->shader, VulkanVertToConfig(vin), &r->config, FALSE));

    PASS_CALL(CreateStaticBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, verts, sizeof(verts), &r->verts));
    PASS_CALL(CreateStaticBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indicies, sizeof(indicies), &r->index));

    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        PASS_CALL(CreateDynamicBuffer(sizeof(sm_mat4f), &r->uniforms[i], VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));
    }

    for (int i = 0; i < SR_MAX_FRAMES_IN_FLIGHT; i++) {
        PASS_CALL(SetBuffer(&r->config, SR_DESC_UNIFORM, (Buffer*)&r->uniforms[i], 0, 0));
    }
    return SR_NO_ERROR;
}

void FractalDrawFrame(FractalRenderer* r, PresentInfo* p, u32 frame) {

}
