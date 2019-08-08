//
// Created by Novak on 23/06/2019.
//

#include "eagle/core/Log.h"
#include "eagle/renderer/vulkan/VulkanRenderTarget.h"
#include "eagle/renderer/vulkan/VulkanHelper.h"

_EAGLE_BEGIN

VulkanRenderTarget::VulkanRenderTarget(uint32_t width, uint32_t height,
                                       const std::vector<RENDER_TARGET_ATTACHMENT> &attachments,
                                       VulkanRenderTargetCreateInfo &info) :
    RenderTarget(width, height, attachments),
    m_info(info),
    m_depthAttachment(std::make_shared<VulkanImageAttachment>()){
    create(width, height);
}

VulkanRenderTarget::~VulkanRenderTarget() {
   cleanup();
}

void VulkanRenderTarget::cleanup() {
    if (m_cleared)
        return;

    EG_CORE_TRACE("Clearing render target!");

    auto destroy_attachment = [&](std::shared_ptr<VulkanImageAttachment> attachment){
        vkDestroyImageView(m_info.device, attachment->view, nullptr);
        vkDestroyImage(m_info.device, attachment->image, nullptr);
        vkFreeMemory(m_info.device, attachment->memory, nullptr);
    };

    VK_CALL vkDestroyFramebuffer(m_info.device, m_framebuffer, nullptr);

    VK_CALL vkDestroyRenderPass(m_info.device, m_renderPass, nullptr);


    for (auto& image : m_images){
        VK_CALL vkDestroySampler(
                m_info.device,
                std::static_pointer_cast<VulkanImageSampler>(image->get_sampler().lock())->sampler,
                nullptr
                );

        auto attachment = std::static_pointer_cast<VulkanImageAttachment>(image->get_attachment().lock());
        destroy_attachment(attachment);
    }

    VK_CALL destroy_attachment(m_depthAttachment);

    EG_CORE_TRACE("Render target cleared!");
    m_cleared = true;
}

void VulkanRenderTarget::create_resources() {

    m_images.clear();
    m_images.resize(m_attachments.size());

    for (size_t i = 0; i < m_images.size(); i++){
        m_images[i] = std::make_shared<VulkanImage>(m_width, m_height);
        auto attachment = std::static_pointer_cast<VulkanImageAttachment>(m_images[i]->get_attachment().lock());
        auto sampler = std::static_pointer_cast<VulkanImageSampler>(m_images[i]->get_sampler().lock());
        create_image_resources(
                        attachment,
                        m_info.colorFormat,
                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                        VK_IMAGE_ASPECT_COLOR_BIT
                        );
        VK_CALL VulkanHelper::create_image_sampler(m_info.device, sampler->sampler, VK_SAMPLER_ADDRESS_MODE_REPEAT);
    }

    auto has_stencil_component = [&](VkFormat format) {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    };

    VkImageAspectFlags depthAspect = VK_IMAGE_ASPECT_DEPTH_BIT;

    if (has_stencil_component(m_info.depthFormat)) {
        depthAspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    create_image_resources(m_depthAttachment, m_info.depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthAspect);
}

void VulkanRenderTarget::create_render_pass() {

    // + 1 for depth attachment
    std::vector<VkAttachmentDescription> attachmentDescriptions(m_images.size() + 1);
    // Color attachment

    for (size_t i = 0; i < attachmentDescriptions.size(); i++){
        attachmentDescriptions[i].samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescriptions[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescriptions[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescriptions[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescriptions[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        if (i != attachmentDescriptions.size() - 1){
            attachmentDescriptions[i].format = m_info.colorFormat;
            attachmentDescriptions[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentDescriptions[i].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        } else {
            attachmentDescriptions[i].format = m_info.depthFormat;
            attachmentDescriptions[i].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            attachmentDescriptions[i].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }
    }

    std::vector<VkAttachmentReference> colorReferences(m_images.size());
    for (size_t i = 0; i < colorReferences.size(); i++){
        colorReferences[i].attachment = i;
        colorReferences[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    VkAttachmentReference depthReference = {};
    depthReference.attachment = colorReferences.size();
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = colorReferences.size();
    subpassDescription.pColorAttachments = colorReferences.data();
    subpassDescription.pDepthStencilAttachment = &depthReference;

    // Use subpass dependencies for layout transitions
    std::vector<VkSubpassDependency> dependencies(attachmentDescriptions.size());
    for (size_t i = 0; i < dependencies.size(); i++){

        dependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        if (i != dependencies.size() - 1){
            //color
            dependencies[i].srcSubpass = VK_SUBPASS_EXTERNAL;
            dependencies[i].dstSubpass = 0;
            dependencies[i].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            dependencies[i].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependencies[i].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            dependencies[i].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        } else {
            //depth
            dependencies[i].srcSubpass = 0;
            dependencies[i].dstSubpass = VK_SUBPASS_EXTERNAL;
            dependencies[i].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependencies[i].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            dependencies[i].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            dependencies[i].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        }
    }

    // Create the actual renderpass
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
    renderPassInfo.pAttachments = attachmentDescriptions.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();

    VK_CALL vkCreateRenderPass(m_info.device, &renderPassInfo, nullptr, &m_renderPass);
}

void VulkanRenderTarget::create_framebuffer() {

    std::vector<VkImageView> attachments(m_images.size());
    for (size_t i = 0; i < attachments.size(); i++){
        attachments[i] = std::static_pointer_cast<VulkanImageAttachment>(m_images[i]->get_attachment().lock())->view;
    }
    attachments.emplace_back(m_depthAttachment->view);

    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = m_renderPass;
    framebufferCreateInfo.attachmentCount = attachments.size();
    framebufferCreateInfo.pAttachments = attachments.data();
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

void VulkanRenderTarget::create_image_resources(std::shared_ptr<VulkanImageAttachment> &image, VkFormat format,
                                                VkImageUsageFlags usageFlags, VkImageAspectFlags aspectMask) {

    VK_CALL VulkanHelper::create_image(m_info.physicalDevice, m_info.device, m_extent.width, m_extent.height,
                                       1, 1, format, VK_IMAGE_TILING_OPTIMAL, usageFlags,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image->image, image->memory);

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = aspectMask;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    VK_CALL VulkanHelper::create_image_view(m_info.device, image->image, image->view,
                                            format, VK_IMAGE_VIEW_TYPE_2D, subresourceRange);

}

std::weak_ptr<Image> VulkanRenderTarget::get_image(size_t index) {
    return m_images[index];
}

std::vector<std::weak_ptr<Image>> VulkanRenderTarget::get_images() {

    std::vector<std::weak_ptr<Image>> images(m_images.size());
    for (size_t i = 0; i < m_images.size(); i++){
        images[i] = m_images[i];
    }
    return images;
}

std::vector<VkClearValue> VulkanRenderTarget::get_clear_values() {
    std::vector<VkClearValue> clearValues(m_images.size() + 1);
    for (uint32_t i = 0; i < m_images.size(); i++){
        clearValues[i].color = {0.0f, 0.0f, 0.0f, 1.0f};
    }
    clearValues[clearValues.size() - 1].depthStencil = {1.0f, 0};
    return clearValues;
}


_EAGLE_END
