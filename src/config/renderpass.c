#include "config.h"
#include "error.h"
#include "init.h"
#include "texture.h"
#include "util.h"
#include <vulkan/vulkan_core.h>


i32 findSupportedFormat(const VkFormat* candidates, u32 numCandidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    
    for (int i = 0; i < numCandidates; i++) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(sr_device.p, candidates[i], &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures && features) == features) {
            return i;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures && features) == features) {
            return i;
        }
    }
    return -1;
}


ErrorCode CreatePass(RenderPass* r, AttachmentConfig* configs, u32 numAttachments) {

    VulkanDevice* d = &sr_device;
    VulkanContext* c = &sr_context;

    SwapChainDetails swapDetails;
    querySwapDetails(&swapDetails, d->p, c->surface);

    //Pick Format
    VkSurfaceFormatKHR format = swapDetails.formats[0];
    for (int i = 0; i < swapDetails.formatCount; i++) {
        if (swapDetails.formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && 
            swapDetails.formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            format = swapDetails.formats[i];
            break;
        }
    }

    free(swapDetails.formats);
    free(swapDetails.modes);

    //Make RenderPass
    
    VkAttachmentDescription attachDescriptions[numAttachments + 1];
    VkAttachmentReference attachReferences[numAttachments + 1];
    VkSubpassDependency dependency;

    {
        VkAttachmentDescription* colorAttachment = &attachDescriptions[0];
        colorAttachment->flags = 0;
        colorAttachment->format = format.format;
        colorAttachment->samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment->storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        colorAttachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        colorAttachment->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment->finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference* colorAttachRef = &attachReferences[0];
        colorAttachRef->attachment = 0;
        colorAttachRef->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }
    for (int i = 1; i <= numAttachments; i++) { 
        switch (configs[i].type) {
            case SR_ATTATCHMENT_DEPTH: {
                const VkFormat depthFormats[] = {
                    VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT
                };

                VkFormat format;
                {
                    int index = findSupportedFormat(depthFormats, ARRAY_SIZE(depthFormats), VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
                    if (index == -1) return SR_CREATE_FAIL;
                    format = depthFormats[index];
                }

                VkAttachmentDescription* depthAttachment = &attachDescriptions[i];
                depthAttachment->flags = 0;
                depthAttachment->format = format; 
                depthAttachment->samples = VK_SAMPLE_COUNT_1_BIT;
                depthAttachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                depthAttachment->storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAttachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                depthAttachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAttachment->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                depthAttachment->finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                VkAttachmentReference* depthReference = &attachReferences[i];
                depthReference->attachment = 1;
                depthReference->layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


                dependency.srcStageMask  |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                dependency.dstStageMask  |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                dependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;
            }
        }
    }

    VkSubpassDescription subDescription = {0};
    subDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subDescription.colorAttachmentCount = numAttachments + 1;
    subDescription.pColorAttachments = attachReferences;

    
    VkRenderPassCreateInfo renderInfo = {0};
    renderInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderInfo.attachmentCount = numAttachments + 1;
    renderInfo.pAttachments = attachDescriptions;
    renderInfo.subpassCount = 1;
    renderInfo.pSubpasses = &subDescription;
    renderInfo.dependencyCount = numAttachments + 1;
    renderInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(d->l, &renderInfo, NULL, &r->pass) != VK_SUCCESS) {
        SR_LOG_WAR("Failed to Create Render Pass");
        return SR_CREATE_FAIL;
    }
    return SR_NO_ERROR;
}



void DestroyPass(RenderPass* r) {
    vkDestroyRenderPass(sr_device.l, r->pass, NULL);
    SR_LOG_DEB("Destroyed RenderPass");  
}
