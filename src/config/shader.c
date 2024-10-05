#include "pipeline.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include <stdio.h>
#include <inttypes.h>
#include <vulkan/vulkan_core.h>


ErrorCode CreateShaderProg(const char* vertex, const char* frag, VulkanShader* s) {
    //Read from file
    FILE* vertFile = fopen(vertex, "rb");
    if (vertFile == NULL) {
        SR_LOG_ERR("Invalid File");
        return SR_INVALID;
    }
    fseek(vertFile, 0, SEEK_END);
    size_t vertSize = ftell(vertFile);
    fseek(vertFile, 0, SEEK_SET);

    char vertmem[vertSize + sizeof(uint32_t)];
    uint32_t* vertRaw = (uint32_t*)((uintptr_t)vertmem & ~0xF);
    if (fread(vertRaw, 1, vertSize, vertFile) < 0) {
        fclose(vertFile);
        SR_LOG_ERR("File read failed");
        return SR_INVALID;
    }
    fclose(vertFile);

    
    FILE* fragFile = fopen(frag, "rb");
    if (fragFile == NULL) {
        SR_LOG_ERR("Invalid File");
        return SR_INVALID;
    }
    fseek(fragFile, 0, SEEK_END);
    size_t fragSize = ftell(fragFile);
    fseek(fragFile, 0, SEEK_SET);

    char fragmem[fragSize + sizeof(uint32_t)];
    uint32_t* fragRaw = (uint32_t*)((uintptr_t)fragmem & ~0xF);
    if (fread(fragRaw, 1, fragSize, fragFile) < 0) {
        fclose(fragFile);
        SR_LOG_ERR("File read failed");
        return SR_INVALID;
    }
    fclose(fragFile);

    //Create Shader Modules
    VkShaderModuleCreateInfo vertInfo = {0};
    vertInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    vertInfo.codeSize = vertSize;
    vertInfo.pCode = vertRaw;

    if (vkCreateShaderModule(sr_device.l, &vertInfo, NULL, &s->vertex) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Create vertex shader Module");
        return SR_CREATE_FAIL;
    }
    
    VkShaderModuleCreateInfo fragInfo = {0};
    fragInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    fragInfo.codeSize = fragSize;
    fragInfo.pCode = fragRaw;

    if (vkCreateShaderModule(sr_device.l, &fragInfo, NULL, &s->fragment) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Create fragment shader Module");
        return SR_CREATE_FAIL;
    }

    SR_LOG_DEB("Shader Modules Created");


    return SR_NO_ERROR;
}
