//
// Created by Novak on 08/08/2020.
//

#include <eagle/renderer/vulkan/VulkanFramebuffer.h>

EG_BEGIN

VulkanFramebuffer::VulkanFramebuffer(const FramebufferCreateInfo &createInfo,
                                     const VulkanFramebufferCreateInfo &nativeFramebufferCreateInfo) :
    Framebuffer(createInfo),
    m_nativeCreateInfo(nativeFramebufferCreateInfo) {
    EG_CORE_TRACE("Constructing a vulkan frame buffer!");
    m_nativeImageAttachments.reserve(createInfo.attachments.size());
    for (auto& attachment : createInfo.attachments){
        m_nativeImageAttachments.emplace_back(std::static_pointer_cast<VulkanImage>(attachment));
    }
    create_framebuffer();
    EG_CORE_TRACE("Vulkan frame buffer constructed!");
}

VulkanFramebuffer::~VulkanFramebuffer() {
    EG_CORE_TRACE("Destroying a vulkan frame buffer!");
    for (auto& framebuffer : m_framebuffers) {
        VK_CALL vkDestroyFramebuffer(m_nativeCreateInfo.device, framebuffer, nullptr);
    }
    EG_CORE_TRACE("Vulkan frame buffer destroyed!");
}


void VulkanFramebuffer::create_framebuffer() {
    EG_CORE_TRACE("Creating a vulkan frame buffer!");

    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = std::static_pointer_cast<VulkanRenderPass>(m_createInfo.renderPass)->native_render_pass();
    framebufferCreateInfo.width = m_createInfo.width;
    framebufferCreateInfo.height = m_createInfo.height;
    framebufferCreateInfo.layers = 1;

    m_framebuffers.resize(m_nativeCreateInfo.imageCount, nullptr);
    for (int i = 0; i < m_framebuffers.size(); i++) {
        std::vector<VkImageView> attachments;
        attachments.reserve(m_nativeImageAttachments.size());
        for (auto &image : m_nativeImageAttachments) {
            attachments.emplace_back(image->native_image_views()[i]);
        }
        framebufferCreateInfo.attachmentCount = attachments.size();
        framebufferCreateInfo.pAttachments = attachments.data();

        VK_CALL vkCreateFramebuffer(m_nativeCreateInfo.device, &framebufferCreateInfo, nullptr, &m_framebuffers[i]);
    }
    EG_CORE_TRACE("Vulkan frame buffer created!");
}

EG_END
