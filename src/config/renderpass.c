#include "config.h"
#include "error.h"
#include "init.h"
#include "log.h"
#include <vulkan/vulkan_core.h>


static ErrorCode findFormat(const VkFormat* candidates, u32 num, VkImageTiling tiling, VkFormatFeatureFlags flags, VkFormat* out) {

    for (int i = 0; i < num; i++) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(sr_device.p, candidates[i], &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && ((props.linearTilingFeatures & flags) == flags)) {
            *out = candidates[i];
            return SR_NO_ERROR;
        }
        if ((tiling == VK_IMAGE_TILING_OPTIMAL) && ((props.optimalTilingFeatures & flags) == flags)) {
            *out = candidates[i];
            return SR_NO_ERROR;
        }
    }

    return SR_LOAD_FAIL;
}

ErrorCode CreateMultiPass(RenderPass* r, SubPass* passes, u32 numPasses, Attachment* configs, u32 numAttachments) {

    VkSubpassDescription subdescriptions[numPasses];
    VkSubpassDependency subdependencies[numPasses - 1];

    VkAttachmentDescription attachDescriptions[numAttachments + 1];
    VkAttachmentReference attachReferences[numAttachments + 1];

    SwapChainDetails swapDetails;
    querySwapDetails(&swapDetails, sr_device.p, sr_context.surface);

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
    }
    for (int i = 1; i <= numAttachments; i++) { 
        switch (configs[i-1].type) {
            case SR_ATTATCHMENT_DEPTH: {
                static const VkFormat depthFormats[] = {
                    VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT
                };

                VkFormat format;
                {
                    ErrorCode e = findFormat(depthFormats, 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, &format);
                    if (e != SR_NO_ERROR) return SR_CREATE_FAIL;
                }

                configs[i - 1].format = format; 

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
                break;
            }
        }
    }

    //construct subpasses
    for (int i = 0; i < numPasses; i++) {
        subdependencies[i] = (VkSubpassDependency){
            .srcSubpass = i == 0 ? VK_SUBPASS_EXTERNAL : i - 1,
            .dstSubpass = i,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
        };
        //single color attachment
        subdescriptions[i] = (VkSubpassDescription) {
            .colorAttachmentCount = 1,
            .pColorAttachments = &attachReferences[0],
            .inputAttachmentCount = 0,
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE,
            .pDepthStencilAttachment = &attachReferences[passes[i].depthAttachment],
        };

        //build up access masks
        
        for (int j = passes[i].firstAttachment; j < passes[i].numAttachments; j++) {
            switch (configs[j].type) {
                case SR_ATTATCHMENT_DEPTH: 
                    subdependencies[i].dstStageMask  |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                    subdependencies[i].srcStageMask  |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                    subdependencies[i].dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                    break;
                default:
                    break;
            }
        }
    }

    VkRenderPassCreateInfo renderInfo = {0};
    renderInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderInfo.attachmentCount = numAttachments + 1;
    renderInfo.pAttachments = attachDescriptions;
    renderInfo.subpassCount = numPasses;
    renderInfo.pSubpasses = subdescriptions;
    renderInfo.dependencyCount = numPasses;
    renderInfo.pDependencies = subdependencies;

    if (vkCreateRenderPass(sr_device.l, &renderInfo, NULL, &r->pass) != VK_SUCCESS) {
        SR_LOG_WAR("Failed to Create Render Pass");
        return SR_CREATE_FAIL;
    }

    r->configs = configs;
    r->numAttachments = numAttachments + 1;
    return SR_NO_ERROR;
}


ErrorCode CreatePass(RenderPass* r, Attachment* configs, u32 numAttachments) {

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
    VkSubpassDependency dependency = {0};

    VkSubpassDescription subDescription = {0};
    subDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subDescription.colorAttachmentCount = 1;

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


        subDescription.pColorAttachments = &attachReferences[0];
    }
    for (int i = 1; i <= numAttachments; i++) { 
        switch (configs[i-1].type) {
            case SR_ATTATCHMENT_DEPTH: {
                static const VkFormat depthFormats[] = {
                    VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT
                };

                VkFormat format;
                {
                    ErrorCode e = findFormat(depthFormats, 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, &format);
                    if (e != SR_NO_ERROR) return SR_CREATE_FAIL;
                }

                configs[i - 1].format = format; 

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

                subDescription.pDepthStencilAttachment = &attachReferences[1];
                break;
            }
        }
    }


    
    VkRenderPassCreateInfo renderInfo = {0};
    renderInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderInfo.attachmentCount = numAttachments + 1;
    renderInfo.pAttachments = attachDescriptions;
    renderInfo.subpassCount = 1;
    renderInfo.pSubpasses = &subDescription;
    renderInfo.dependencyCount = 1;
    renderInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(d->l, &renderInfo, NULL, &r->pass) != VK_SUCCESS) {
        SR_LOG_WAR("Failed to Create Render Pass");
        return SR_CREATE_FAIL;
    }

    r->configs = configs;
    r->numAttachments = numAttachments + 1;
    return SR_NO_ERROR;
}


void DestroyPass(RenderPass* r) {
    vkDestroyRenderPass(sr_device.l, r->pass, NULL);
    SR_LOG_DEB("Destroyed RenderPass");  
}
