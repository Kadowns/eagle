//
// Created by Novak on 23/06/2019.
//

#include "eagle/core/Log.h"
#include "eagle/core/renderer/vulkan/VulkanRenderTarget.h"
#include "eagle/core/renderer/vulkan/VulkanHelper.h"

EG_BEGIN

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

    auto destroy_attachment = [&](Reference<VulkanImageAttachment> attachment){
        vkDestroyImageView(m_info.device, attachment->view, nullptr);
        vkDestroyImage(m_info.device, attachment->image, nullptr);
        vkFreeMemory(m_info.device, attachment->memory, nullptr);
    };

    VK_CALL vkDestroyFramebuffer(m_info.device, m_framebuffer, nullptr);


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

void VulkanRenderTarget::create_framebuffer() {

    std::vector<VkImageView> attachments(m_images.size());
    for (size_t i = 0; i < attachments.size(); i++){
        attachments[i] = std::static_pointer_cast<VulkanImageAttachment>(m_images[i]->get_attachment().lock())->view;
    }
    attachments.emplace_back(m_depthAttachment->view);

    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = m_info.renderPass;
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
    //create_render_pass();
    create_framebuffer();
    m_cleared = false;
}

void VulkanRenderTarget::create_image_resources(Reference<VulkanImageAttachment> &image, VkFormat format,
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

Handle<Image> VulkanRenderTarget::get_image(size_t index) {
    return m_images[index];
}

std::vector<Handle<Image>> VulkanRenderTarget::get_images() {

    std::vector<Handle<Image>> images(m_images.size());
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


EG_END
