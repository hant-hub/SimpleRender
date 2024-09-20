#include "config.h"
#include "init.h"
#include <vulkan/vulkan_core.h>



ErrorCode CreatePass(RenderPass* r) {

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
    VkAttachmentDescription colorAttachment = {0};
    colorAttachment.format = format.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachRef = {0};
    colorAttachRef.attachment = 0;
    colorAttachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subDescription = {0};
    subDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subDescription.colorAttachmentCount = 1;
    subDescription.pColorAttachments = &colorAttachRef;

    VkSubpassDependency dependency = {0};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    
    VkRenderPassCreateInfo renderInfo = {0};
    renderInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderInfo.attachmentCount = 1;
    renderInfo.pAttachments = &colorAttachment;
    renderInfo.subpassCount = 1;
    renderInfo.pSubpasses = &subDescription;
    renderInfo.dependencyCount = 1;
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
