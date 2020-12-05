//
// Created by Novak on 23/06/2019.
//

#include "eagle/core/Log.h"
#include "eagle/core/renderer/vulkan/VulkanRenderTarget.h"
#include "eagle/core/renderer/vulkan/VulkanHelper.h"

EG_BEGIN

VulkanCustomRenderTarget::VulkanCustomRenderTarget(uint32_t width, uint32_t height, VkRenderPass &renderPass,
                                                   VulkanRenderTargetCreateInfo &info) :
    m_renderPass(renderPass), m_info(info){
    create(width, height);
}

VulkanCustomRenderTarget::~VulkanCustomRenderTarget() {
   cleanup();
}

void VulkanCustomRenderTarget::cleanup() {
    if (m_cleared)
        return;

    EG_CORE_TRACE("Clearing render target!");

    auto destroy_attachment = [&](Reference<VulkanImageAttachment>& attachment){
        vkDestroyImageView(m_info.device, attachment->view, nullptr);
        vkDestroyImage(m_info.device, attachment->image, nullptr);
        vkFreeMemory(m_info.device, attachment->memory, nullptr);
    };

    VK_CALL vkDestroyFramebuffer(m_info.device, m_framebuffer, nullptr);
    auto attachment = std::static_pointer_cast<VulkanImageAttachment>(m_image->get_attachment().lock());
    auto sampler = std::static_pointer_cast<VulkanImageSampler>(m_image->get_sampler().lock());

    VK_CALL destroy_attachment(m_depthAttachment);
    VK_CALL destroy_attachment(attachment);

    VK_CALL vkDestroySampler(m_info.device, sampler->sampler, nullptr);

    EG_CORE_TRACE("Render target cleared!");
    m_cleared = true;
}

void VulkanCustomRenderTarget::create_resources() {
    EG_CORE_TRACE("Creating custom render target resources!");


    auto attachment = std::static_pointer_cast<VulkanImageAttachment>(m_image->get_attachment().lock());
    auto sampler = std::static_pointer_cast<VulkanImageSampler>(m_image->get_sampler().lock());
    create_image_resources(
            attachment,
            m_info.colorFormat,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_IMAGE_ASPECT_COLOR_BIT
    );

    VK_CALL VulkanHelper::create_image_sampler(m_info.device, sampler->sampler, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_FILTER_LINEAR);


    VkImageAspectFlags depthAspect = VK_IMAGE_ASPECT_DEPTH_BIT;

    if (m_info.depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || m_info.depthFormat == VK_FORMAT_D24_UNORM_S8_UINT) {
        depthAspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    m_depthAttachment = std::make_shared<VulkanImageAttachment>();
    create_image_resources(m_depthAttachment, m_info.depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthAspect);
    EG_CORE_TRACE("Custom render target resources created!");
}

void VulkanCustomRenderTarget::create_framebuffer() {

    std::vector<VkImageView> attachments;
    attachments.reserve(2);
    attachments.emplace_back(std::static_pointer_cast<VulkanImageAttachment>(m_image->get_attachment().lock())->view);
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

void VulkanCustomRenderTarget::create(uint32_t width, uint32_t height) {
    if (!m_cleared)
        return;

    if (!m_image){
        m_image = std::make_shared<VulkanImage>(DescriptorType::SAMPLED_IMAGE, m_extent.width, m_extent.height);
    }else {
        m_image->resize(width, height);
    }

    m_extent.width = width;
    m_extent.height = height;
    create_resources();
    create_framebuffer();
    m_cleared = false;
}

void VulkanCustomRenderTarget::create_image_resources(Reference<VulkanImageAttachment> &image, VkFormat format,
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

Handle<Image> VulkanCustomRenderTarget::get_image() {
    return m_image;
}

std::vector<VkClearValue> VulkanCustomRenderTarget::get_clear_values() {
    std::vector<VkClearValue> clearValues(2);
    clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    return clearValues;
}

//-----------------------------------------------------------------------------------------
//main target------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

VulkanMainRenderTarget::VulkanMainRenderTarget(const VulkanRenderTargetCreateInfo &info, VkImage image,
                                               VkRenderPass &renderPass, uint32_t width, uint32_t height) :
                                               m_info(info), m_renderPass(renderPass) {

    m_extent.width = width;
    m_extent.height = height;

    m_image = std::make_shared<VulkanImage>(DescriptorType::SAMPLED_IMAGE, width, height);
    auto attachment = std::static_pointer_cast<VulkanImageAttachment>(m_image->get_attachment().lock());
    attachment->image = image;

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    VulkanHelper::create_image_view(
            info.device,
            image,
            attachment->view,
            info.colorFormat,
            VK_IMAGE_VIEW_TYPE_2D,
            subresourceRange
            );

    VkImageAspectFlags depthAspect = VK_IMAGE_ASPECT_DEPTH_BIT;

    if (m_info.depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || m_info.depthFormat == VK_FORMAT_D24_UNORM_S8_UINT) {
        depthAspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    m_depthAttachment = std::make_shared<VulkanImageAttachment>();

    VK_CALL VulkanHelper::create_image(m_info.physicalDevice, m_info.device, m_extent.width, m_extent.height,
                                       1, 1, m_info.depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depthAttachment->image, m_depthAttachment->memory);

    subresourceRange = {};
    subresourceRange.aspectMask = depthAspect;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    VK_CALL VulkanHelper::create_image_view(m_info.device, m_depthAttachment->image, m_depthAttachment->view,
                                            m_info.depthFormat, VK_IMAGE_VIEW_TYPE_2D, subresourceRange);


    std::vector<VkImageView> attachments;
    attachments.reserve(2);
    attachments.emplace_back(std::static_pointer_cast<VulkanImageAttachment>(m_image->get_attachment().lock())->view);
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

VulkanMainRenderTarget::~VulkanMainRenderTarget() {

    auto attachment = std::static_pointer_cast<VulkanImageAttachment>(m_image->get_attachment().lock());

    VK_CALL vkDestroyImageView(m_info.device, attachment->view, nullptr);
    VK_CALL vkDestroyImageView(m_info.device, m_depthAttachment->view, nullptr);
    VK_CALL vkDestroyImage(m_info.device, m_depthAttachment->image, nullptr);
    VK_CALL vkFreeMemory(m_info.device, m_depthAttachment->memory, nullptr);

    VK_CALL vkDestroyFramebuffer(m_info.device, m_framebuffer, nullptr);

}

Handle<Image> VulkanMainRenderTarget::get_image() {
    return m_image;
}

VkRenderPass& VulkanMainRenderTarget::get_render_pass() {
    return m_renderPass;
}

VkFramebuffer& VulkanMainRenderTarget::get_framebuffer() {
    return m_framebuffer;
}

VkExtent2D& VulkanMainRenderTarget::get_extent() {
    return m_extent;
}

std::vector<VkClearValue> VulkanMainRenderTarget::get_clear_values() {
    std::vector<VkClearValue> clearValues(2);
    clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    return clearValues;
}


EG_END


