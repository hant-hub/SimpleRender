#include "command.h"
#include "config.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include "pipeline.h"
#include "texture.h"
#include "util.h"
#include <vulkan/vulkan_core.h>


static uint32_t clampi(uint32_t in, uint32_t min, uint32_t max) {
    const uint32_t t = in < min ? min : in;
    return t > max ? max : t;
}

static ErrorCode findFormat(const VkFormat* candidates, u32 num, VkImageTiling tiling, VkFormatFeatureFlags flags, VkFormat* out) {

    for (int i = 0; i < num; i++) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(sr_device.p, candidates[i], &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures && flags) == flags) {
            *out = candidates[i];
            return SR_NO_ERROR;
        }
        if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures && flags) == flags) {
            *out = candidates[i];
            return SR_NO_ERROR;
        }
    }

    return SR_LOAD_FAIL;
}

//also makes renderpass attachments
ErrorCode CreateFrameBuffers(VulkanCommand* c, VulkanDevice* d, SwapChain*s, RenderPass* r) {

    //Retrieve Swapchain Images
    VkImage images[s->imgCount];
    vkGetSwapchainImagesKHR(d->l, s->swapChain, &s->imgCount, images);

    //Create Image Views for swapchain
    s->views = (VkImageView*)malloc(sizeof(VkImageView) * s->imgCount);
    for (int i = 0; i < s->imgCount; i++) {
        VkImageViewCreateInfo viewInfo = {0};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = images[i];

        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = s->format.format;

        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; 
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(d->l, &viewInfo, NULL, &s->views[i]) != VK_SUCCESS) {
            for (int j = 0; j <= i; j--){
                vkDestroyImageView(d->l, s->views[i], NULL);
            }
            free(s->views);


            SR_LOG_ERR("Failed to make swap Image View");
            return SR_CREATE_FAIL;
        }
    }
    SR_LOG_DEB("\tImage Views Created");

    //Create Attachments
    Attachment* attach = r->configs;
    for (int i = 0; i < r->numAttachments - 1; i++) {
        switch (attach[i].type) {
            case SR_ATTATCHMENT_DEPTH:
                {
                    CreateImage(c, &attach[i].image,
                            (ImageConfig){
                            s->extent.width,
                            s->extent.height,
                            VK_IMAGE_TILING_OPTIMAL,
                            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                            VK_IMAGE_ASPECT_DEPTH_BIT,
                            attach[i].format 
                            });
                }
                break;
        }

    }

    //create frame buffers
    s->buffers = (VkFramebuffer*)malloc(s->imgCount*sizeof(VkFramebuffer));
    for (size_t i = 0; i < s->imgCount; i++) {
        VkImageView attachments[r->numAttachments];
        attachments[0] = s->views[i];
        for (int j = 1; j <= r->numAttachments - 1; j++) {
            attachments[j] = attach[j - 1].image.view;
        }

        VkFramebufferCreateInfo createInfo = {0};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = r->pass;
        createInfo.attachmentCount = r->numAttachments;
        createInfo.pAttachments = attachments;
        createInfo.width = s->extent.width;
        createInfo.height = s->extent.height;
        createInfo.layers = 1;

        if (vkCreateFramebuffer(d->l, &createInfo, NULL, &s->buffers[i]) != VK_SUCCESS) {
            SR_LOG_ERR("Failed to create Framebuffer");
            for (int j = 0; j <= i; j--){
                vkDestroyFramebuffer(d->l, s->buffers[j], NULL);
            }
            free(s->buffers);
            return SR_CREATE_FAIL;
        }
    }
    SR_LOG_DEB("\tFrameBuffers Created");


    return SR_NO_ERROR;
}

ErrorCode CreateSwapChain(RenderPass* r, SwapChain* s, VulkanCommand* cmd, VkSwapchainKHR old) {
    VulkanDevice* d = &sr_device;
    VulkanContext* c = &sr_context;

    SwapChainDetails swapDetails;
    querySwapDetails(&swapDetails, d->p, c->surface);

    //Pick Format
    s->format = swapDetails.formats[0];
    for (int i = 0; i < swapDetails.formatCount; i++) {
        if (swapDetails.formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && 
            swapDetails.formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            s->format = swapDetails.formats[i];
            break;
        }
    }
    //Pick Mode
    s->mode = VK_PRESENT_MODE_FIFO_KHR;
    for (int i = 0; i < swapDetails.modeCount; i++) {
        if (swapDetails.modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) { 
            s->mode = swapDetails.modes[i];
            break;
        }
    }

    //pick Extent
    VkSurfaceCapabilitiesKHR capabilities = swapDetails.capabilities;
    if (capabilities.currentExtent.width != UINT32_MAX) {
        s->extent = capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(c->w, &width, &height);

        s->extent.width = (uint32_t)clampi((uint32_t)width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        s->extent.height = (uint32_t)clampi((uint32_t)height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }
    s->imgCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && s->imgCount > capabilities.maxImageCount) {
        s->imgCount = capabilities.maxImageCount;
    }
    free(swapDetails.formats);
    free(swapDetails.modes);

    //Create Swapchain
    VkSwapchainCreateInfoKHR swapInfo = {0};
    swapInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapInfo.surface = c->surface;

    swapInfo.minImageCount = s->imgCount;
    swapInfo.imageFormat = s->format.format;
    swapInfo.imageColorSpace = s->format.colorSpace;
    swapInfo.imageExtent = s->extent;
    swapInfo.imageArrayLayers = 1;
    swapInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    QueueFamilyIndicies indicies = findQueueFamilies(d->p, c->surface);
    uint32_t queueFamilyIndicies[] = {indicies.graphicsFamily.val, indicies.presentFamily.val};

    if (indicies.graphicsFamily.val != indicies.presentFamily.val) {
        swapInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapInfo.queueFamilyIndexCount = 2;
        swapInfo.pQueueFamilyIndices = queueFamilyIndicies;
    } else {
        swapInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapInfo.queueFamilyIndexCount = 0;
        swapInfo.pQueueFamilyIndices = NULL;
    }

    swapInfo.preTransform = capabilities.currentTransform;
    swapInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapInfo.presentMode = s->mode;
    swapInfo.clipped = VK_TRUE;
    swapInfo.oldSwapchain = old;

    VkResult result = vkCreateSwapchainKHR(d->l, &swapInfo, NULL, &s->swapChain);
    if (result != VK_SUCCESS) {
        SR_LOG_ERR("Failed to Create SwapChain");
        SR_LOG_ERR("Error Code %d", result == VK_ERROR_NATIVE_WINDOW_IN_USE_KHR); 
        return SR_CREATE_FAIL;
    }
    SR_LOG_DEB("SwapChain Created");

    CreateFrameBuffers(cmd, d, s, r);
    return SR_NO_ERROR;
}





void DestroySwapChain(SwapChain* s, RenderPass* r) {
    VkDevice l = sr_device.l;
    for (int i = 0; i < s->imgCount; i++) {
        vkDestroyImageView(l, s->views[i], NULL);
    }
    

    if (s->buffers != NULL) {
        for (int i = 0; i < s->imgCount; i++) {
            vkDestroyFramebuffer(l, s->buffers[i], NULL);
        }
        free(s->buffers);
        SR_LOG_DEB("\tFrameBuffers Destroyed");
    }

    for (int i = 0; i < r->numAttachments - 1; i++) {
        DestroyImage(&r->configs[i].image);
    }

    free(s->views);
    SR_LOG_DEB("\tImage Views Destroyed");


    vkDestroySwapchainKHR(l, s->swapChain, NULL);
    SR_LOG_DEB("SwapChain Destroyed");
}
