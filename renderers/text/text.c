#include "text.h"

#include "config.h"
#include "error.h"
#include "memory.h"
#include "texture.h"
#include "util.h"
#include "vec2.h"
#include "vec4.h"
#include <vulkan/vulkan_core.h>

typedef struct {
    sm_vec2f pos;
    sm_vec2f size;
    u32 c;
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
        {SR_DESC_STORAGE, VK_SHADER_STAGE_VERTEX_BIT, 0},
        //{SR_DESC_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1}
    };
    PASS_CALL(CreateDescriptorSetConfig(&r->config, descriptorConfigs, ARRAY_SIZE(descriptorConfigs)));
    
    AttrConfig vconfig[] = {
        {.rate = VK_VERTEX_INPUT_RATE_VERTEX, .format = VK_FORMAT_R32G32_SFLOAT, .size = sizeof(sm_vec2f)},
        {.rate = VK_VERTEX_INPUT_RATE_VERTEX, .format = VK_FORMAT_R32G32_SFLOAT, .size = sizeof(sm_vec2f)},
        {.rate = VK_VERTEX_INPUT_RATE_VERTEX, .format = VK_FORMAT_R32_UINT,      .size = sizeof(u32)}
    };
    VkVertexInputBindingDescription binds[1];
    VkVertexInputAttributeDescription attrs[3];
    PASS_CALL(CreateVertAttr(attrs, binds, vconfig, 3));


    VulkanVertexInput vin = {
        .attrs = attrs,
        .binding = binds[0],
        .size = 3
    };
    PASS_CALL(CreatePipelineConfig(&r->shader, VulkanVertToConfig(vin), &r->config));

    PASS_CALL(CreatePass(&r->pass, NULL, 0));
//    PASS_CALL(CreateSwapChain(&r->pass, &r->swap, VK_NULL_HANDLE));
    PASS_CALL(CreatePipeline(&r->shader, &r->config, &r->pipeline, &r->pass)); 
    
    LoadFont(&r->fdata);
    PASS_CALL(CreateStaticBuffer(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, &r->fdata, sizeof(FontData), &r->font));

    //todo, fix SetBuffer to be non sprite specific
    //PASS_CALL(SetBuffer(&r->config, SR_DESC_STORAGE, &r->font, sizeof(FontData), 0));
    return SR_NO_ERROR;
}

void TextDestroy(TextRenderer* r) {
    vkDeviceWaitIdle(sr_device.l);

    DestroyTexture(&r->fdata.atlas);

    DestroyStaticBuffer(&r->font);
    DestroyPipeline(&r->pipeline);
    DestroyShaderProg(&r->shader);
    DestroyPipelineConfig(&r->config);
    DestroyPass(&r->pass);
    VkDevice d = sr_device.l;
    DestroySwapChain(&r->swap, &r->pass);
}

ErrorCode UpdateText(TextRenderer* r) {
    return SR_NO_ERROR;
}

void TextDrawFrame(TextRenderer* r) {
}
