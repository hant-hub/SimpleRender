#include "texture.h"
#include "command.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include "memory.h"
#include "fcntl.h"
#include <stdio.h>
#include <string.h>
#include <vulkan/vulkan_core.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void TransitionImageLayout(VulkanDevice* d, VulkanCommand* c, VkImage img, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer cmd = beginSingleTimeCommand(c->pool);
    
    VkImageMemoryBarrier barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = img;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;


    VkPipelineStageFlags srcStage;
    VkPipelineStageFlags dstStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }


    vkCmdPipelineBarrier(
            cmd,
            srcStage,  dstStage,
            0,
            0,  NULL,
            0,  NULL,
            1,  &barrier
            );


    endSingleTimeCommand(cmd, c->pool);
}

ErrorCode LoadTexture(VulkanCommand* c, Texture* t, const char* path) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path,
                                &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    if (pixels == NULL) 
        return SR_LOAD_FAIL;

    VkDeviceSize imgSize = texWidth * texHeight * 4;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;

    CreateBuffer(imgSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             &stagingBuffer, &stagingMemory); 

    CreateImage(c, &t->image, texWidth, texHeight);

    void* data;
    vkMapMemory(sr_device.l, stagingMemory, 0, imgSize, 0, &data);
    memcpy(data, pixels, imgSize);
    vkUnmapMemory(sr_device.l, stagingMemory);

    stbi_image_free(pixels);

    //transfer image
    TransitionImageLayout(&sr_device, c, t->image.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    VkCommandBuffer cmd = beginSingleTimeCommand(c->pool);

    VkBufferImageCopy region = {0};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = (VkOffset3D){0, 0, 0};
    region.imageExtent = (VkExtent3D){
        texWidth,
        texHeight,
        1
    };

    vkCmdCopyBufferToImage(cmd, stagingBuffer, t->image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommand(cmd, c->pool);


    TransitionImageLayout(&sr_device, c, t->image.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    
    vkDestroyBuffer(sr_device.l, stagingBuffer, NULL); 
    vkFreeMemory(sr_device.l, stagingMemory, NULL);

    VkPhysicalDeviceProperties devProps = {0};
    vkGetPhysicalDeviceProperties(sr_device.p, &devProps);


    //make sampler
    VkSamplerCreateInfo samplerInfo = {0};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    
    //set to maximum quality
    samplerInfo.maxAnisotropy = devProps.limits.maxSamplerAnisotropy;

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(sr_device.l, &samplerInfo, NULL, &t->sampler) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to create Texture Sampler");
        return SR_CREATE_FAIL;
    }

    return SR_NO_ERROR;
}


ErrorCode CreateImage(VulkanCommand* c, Image* t, size_t width, size_t height){

    VkDeviceSize imgSize = width * height * 4;



    VkImageCreateInfo imgInfo = {0};
    imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imgInfo.imageType = VK_IMAGE_TYPE_2D;
    imgInfo.extent = (VkExtent3D) {(uint32_t)width, (uint32_t)height, 1};
    imgInfo.mipLevels = 1;
    imgInfo.arrayLayers = 1;

    imgInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    imgInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imgInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imgInfo.flags = 0;

    if (vkCreateImage(sr_device.l, &imgInfo, NULL, &t->image) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Create Texture Image");
        return SR_CREATE_FAIL;
    }

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(sr_device.l, t->image, &memReq);

    VkMemoryAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;

    if (findMemoryType(memReq.memoryTypeBits, sr_device.p, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &allocInfo.memoryTypeIndex) != SR_NO_ERROR) {
        SR_LOG_ERR("Failed to Find correct Memory");
        return SR_CREATE_FAIL;
    }

    if (vkAllocateMemory(sr_device.l, &allocInfo, NULL, &t->mem) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Allocate Memory");
        return SR_CREATE_FAIL;
    }


    vkBindImageMemory(sr_device.l, t->image, t->mem, 0);



    //image view
    VkImageViewCreateInfo viewInfo = {0};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = t->image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(sr_device.l, &viewInfo, NULL, &t->view) != VK_SUCCESS) {
        SR_LOG_ERR("Failed to create Image View");
        return SR_CREATE_FAIL;
    }

    return SR_NO_ERROR;
}



void DestroyTexture(Texture* t) {
    VkDevice d = sr_device.l;
    DestroyImage(&t->image);
    vkDestroySampler(d, t->sampler, NULL);
}

void DestroyImage(Image* i) {
    VkDevice d = sr_device.l;
    vkDeviceWaitIdle(d);
    vkDestroyImageView(d, i->view, NULL);
    vkDestroyImage(d, i->image, NULL);
    vkFreeMemory(d, i->mem, NULL);
}

