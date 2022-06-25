//
// Created by Novak on 08/08/2020.
//

#include <eagle/renderer/vulkan/vulkan_framebuffer.h>

namespace eagle {

VulkanFramebuffer::VulkanFramebuffer(const FramebufferCreateInfo &createInfo,
                                     const VulkanFramebufferCreateInfo &nativeFramebufferCreateInfo) :
    Framebuffer(createInfo),
    m_nativeCreateInfo(nativeFramebufferCreateInfo) {
    EG_TRACE("eagle","Constructing a vulkan frame buffer!");
    create_framebuffer();
    EG_TRACE("eagle","Vulkan frame buffer constructed!");
}

VulkanFramebuffer::~VulkanFramebuffer() {
    EG_TRACE("eagle","Destroying a vulkan frame buffer!");
    for (auto& framebuffer : m_framebuffers) {
        VK_CALL vkDestroyFramebuffer(m_nativeCreateInfo.device, framebuffer, nullptr);
    }
    EG_TRACE("eagle","Vulkan frame buffer destroyed!");
}


void VulkanFramebuffer::create_framebuffer() {
    EG_TRACE("eagle","Creating a vulkan frame buffer!");

    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = std::static_pointer_cast<VulkanRenderPass>(m_createInfo.renderPass)->native_render_pass();
    framebufferCreateInfo.width = m_createInfo.width;
    framebufferCreateInfo.height = m_createInfo.height;
    framebufferCreateInfo.layers = 1;

    m_framebuffers.resize(m_nativeCreateInfo.imageCount);
    for (int i = 0; i < m_framebuffers.size(); i++) {
        std::vector<VkImageView> attachments;
        attachments.reserve(m_createInfo.attachments.size());
        for (auto &image : m_createInfo.attachments) {
            attachments.emplace_back(std::static_pointer_cast<VulkanImage>(image)->native_view()->native_image_view(i));
        }
        framebufferCreateInfo.attachmentCount = attachments.size();
        framebufferCreateInfo.pAttachments = attachments.data();

        VK_CALL vkCreateFramebuffer(m_nativeCreateInfo.device, &framebufferCreateInfo, nullptr, &m_framebuffers[i]);
    }
    EG_TRACE("eagle","Vulkan frame buffer created!");
}

}
