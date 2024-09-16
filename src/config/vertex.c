#include "config.h"
#include "error.h"
#include "log.h"
#include "memory.h"
#include "vec2.h"



ErrorCode CreateVertAttr(VkVertexInputAttributeDescription* attrOut, VkVertexInputBindingDescription* bindOut, AttrConfig* configs, u32 numAttrs) {
    u32 stride = 0;
    for (u32 i = 0; i < numAttrs; i++) {
        AttrConfig c = configs[i];
        attrOut[i] = (VkVertexInputAttributeDescription){
                .format = c.format,
                .binding = i,
                .location = i,
            };
        switch (c.format) {
            case VK_FORMAT_R32_SFLOAT: {
                stride += sizeof(f32);
                break;
            }
            case VK_FORMAT_R32G32_SFLOAT: {
                stride += sizeof(sm_vec2f);
                break;
            }
            case VK_FORMAT_R32G32B32_SFLOAT: {
                stride += sizeof(sm_vec3f);
                break;
            }
            case VK_FORMAT_R32G32B32A32_SFLOAT: {
                stride += sizeof(sm_vec4f);
                break;
            }

            default: {
                SR_LOG_ERR("invalid format %d", c.format);
                return SR_CREATE_FAIL;
            }
        }
    }
    
    *bindOut = (VkVertexInputBindingDescription) {
        .binding = 0,
        .stride = stride,
        .inputRate = configs[0].rate
    };

    return SR_NO_ERROR;
}




ErrorCode MultiCreateVertAttr(VkVertexInputAttributeDescription* attrOut, VkVertexInputBindingDescription* bindOut, AttrConfig* configs, u32 numAttrs) {

    for (u32 i = 0; i < numAttrs; i++) {
        AttrConfig c = configs[i];
        attrOut[i] = (VkVertexInputAttributeDescription){
                .format = c.format,
                .binding = i,
                .location = i,
            };
        bindOut[i] = (VkVertexInputBindingDescription){
                .binding = i,
                .stride = c.size,
                .inputRate = c.rate
            };
    }

    return SR_NO_ERROR;
}



