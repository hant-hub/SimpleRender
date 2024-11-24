#include "config.h"
#include "error.h"
#include "log.h"
#include "memory.h"
#include "vec2.h"
#include <vulkan/vulkan_core.h>



ErrorCode CreateVertAttr(VkVertexInputAttributeDescription* attrOut, VkVertexInputBindingDescription* bindOut, AttrConfig* configs, u32 numAttrs) {
    u32 stride = 0;
    for (u32 i = 0; i < numAttrs; i++) {
        AttrConfig c = configs[i];
        attrOut[i] = (VkVertexInputAttributeDescription){
                .format = c.format,
                .binding = 0,
                .location = i,
                .offset = stride
            };
        
        stride += c.size;
    }
    bindOut[0] = (VkVertexInputBindingDescription) {
        .binding = 0,
        .stride = stride,
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
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



