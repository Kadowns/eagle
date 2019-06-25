//
// Created by Novak on 23/06/2019.
//

#include "eagle/core/Log.h"
#include "eagle/renderer/vulkan/VulkanRenderTarget.h"
#include "eagle/renderer/vulkan/VulkanHelper.h"

_EAGLE_BEGIN

VulkanRenderTarget::VulkanRenderTarget(uint32_t width, uint32_t height, VulkanRenderTargetCreateInfo &info) :
    m_info(info),
    m_color(std::make_shared<VulkanImage>()),
    m_depth(std::make_shared<VulkanImage>()){
    create(width, height);
}

VulkanRenderTarget::~VulkanRenderTarget() {
   cleanup();
}

void VulkanRenderTarget::cleanup() {
    if (m_cleared)
        return;

    EG_CORE_TRACE("Clearing render target!");
    VK_CALL vkDestroyFramebuffer(m_info.device, m_framebuffer, nullptr);

    VK_CALL vkDestroyRenderPass(m_info.device, m_renderPass, nullptr);

    VK_CALL vkDestroySampler(m_info.device, m_color->sampler, nullptr);

    VK_CALL vkDestroyImageView(m_info.device, m_color->view, nullptr);
    VK_CALL vkDestroyImageView(m_info.device, m_depth->view, nullptr);

    VK_CALL vkDestroyImage(m_info.device, m_color->image, nullptr);
    VK_CALL vkDestroyImage(m_info.device, m_depth->image, nullptr);

    VK_CALL vkFreeMemory(m_info.device, m_color->memory, nullptr);
    VK_CALL vkFreeMemory(m_info.device, m_depth->memory, nullptr);
    EG_CORE_TRACE("Render target cleared!");
    m_cleared = true;
}

void VulkanRenderTarget::create_resources() {
//color
    VK_CALL VulkanHelper::create_image(m_info.physicalDevice, m_info.device, m_extent.width, m_extent.height,
                                       1, 1, m_info.colorFormat, VK_IMAGE_TILING_OPTIMAL,
                                       VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_color->image, m_color->memory);

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;


    VK_CALL VulkanHelper::create_image_view(m_info.device, m_color->image, m_color->view,
                                            m_info.colorFormat, VK_IMAGE_VIEW_TYPE_2D,
                                            subresourceRange);


    auto has_stencil_component = [&](VkFormat format) {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    };

    subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    if (has_stencil_component(m_info.depthFormat)) {
        subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    VK_CALL VulkanHelper::create_image(m_info.physicalDevice, m_info.device, m_extent.width, m_extent.height,
                                       1, 1, m_info.depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depth->image,
                                       m_depth->memory);

    VK_CALL VulkanHelper::create_image_view(m_info.device, m_depth->image, m_depth->view,
                                            m_info.depthFormat, VK_IMAGE_VIEW_TYPE_2D, subresourceRange);


    //sampler
    VK_CALL VulkanHelper::create_image_sampler(m_info.device, m_color->sampler, VK_SAMPLER_ADDRESS_MODE_REPEAT);
}

void VulkanRenderTarget::create_render_pass() {

    std::array<VkAttachmentDescription, 2> attchmentDescriptions = {};
    // Color attachment
    attchmentDescriptions[0].format = VK_FORMAT_R8G8B8A8_UNORM;
    attchmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attchmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attchmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    // Depth attachment
    attchmentDescriptions[1].format = m_info.depthFormat;
    attchmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attchmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchmentDescriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ;

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;
    subpassDescription.pDepthStencilAttachment = &depthReference;

    // Use subpass dependencies for layout transitions
    std::array<VkSubpassDependency, 2> dependencies = {};

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


    // Create the actual renderpass
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attchmentDescriptions.size());
    renderPassInfo.pAttachments = attchmentDescriptions.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();


    VK_CALL vkCreateRenderPass(m_info.device, &renderPassInfo, nullptr, &m_renderPass);
}

void VulkanRenderTarget::create_framebuffer() {

    VkImageView attachments[2];
    attachments[0] = m_color->view;
    attachments[1] = m_depth->view;

    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = m_renderPass;
    framebufferCreateInfo.attachmentCount = 2;
    framebufferCreateInfo.pAttachments = attachments;
    framebufferCreateInfo.width = m_extent.width;
    framebufferCreateInfo.height = m_extent.height;
    framebufferCreateInfo.layers = 1;

    VK_CALL vkCreateFramebuffer(m_info.device, &framebufferCreateInfo, nullptr, &m_framebuffer);
}

void VulkanRenderTarget::create(uint32_t width, uint32_t height) {
    if (!m_cleared)
        return;
    m_extent.width = width;
    m_extent.height = height;
    create_resources();
    create_render_pass();
    create_framebuffer();
    m_cleared = false;
}


_EAGLE_END
