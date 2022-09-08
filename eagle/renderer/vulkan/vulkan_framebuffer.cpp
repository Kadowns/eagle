//
// Created by Novak on 08/08/2020.
//

#include <eagle/renderer/vulkan/vulkan_framebuffer.h>

namespace eagle {

VulkanFramebuffer::VulkanFramebuffer(const FramebufferCreateInfo &createInfo,
                                     const VulkanFramebufferCreateInfo &nativeFramebufferCreateInfo) :
    Framebuffer(createInfo),
    m_nativeCreateInfo(nativeFramebufferCreateInfo) {
    create_framebuffer();
}

VulkanFramebuffer::~VulkanFramebuffer() {
    for (auto& framebuffer : m_framebuffers) {
        vkDestroyFramebuffer(m_nativeCreateInfo.device, framebuffer, nullptr);
    }
    EG_TRACE("eagle","Vulkan frame buffer destroyed!");
}


void VulkanFramebuffer::create_framebuffer() {
    EG_TRACE("eagle","Creating a vulkan frame buffer!");

    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = ((VulkanRenderPass*)m_createInfo.renderPass)->native_render_pass();
    framebufferCreateInfo.width = m_createInfo.width;
    framebufferCreateInfo.height = m_createInfo.height;
    framebufferCreateInfo.layers = 1;

    m_framebuffers.resize(m_nativeCreateInfo.frameCount);
    for (int i = 0; i < m_framebuffers.size(); i++) {
        std::vector<VkImageView> attachments;
        attachments.reserve(m_createInfo.attachments.size());
        for (auto &image : m_createInfo.attachments) {
            attachments.emplace_back(((VulkanImage*)image.get())->native_view()->native_image_view(i));
        }
        framebufferCreateInfo.attachmentCount = attachments.size();
        framebufferCreateInfo.pAttachments = attachments.data();

        vkCreateFramebuffer(m_nativeCreateInfo.device, &framebufferCreateInfo, nullptr, &m_framebuffers[i]);
    }
}

VkFramebuffer VulkanFramebuffer::current_native_framebuffer() {
    return m_framebuffers[*m_nativeCreateInfo.currentFrame % m_framebuffers.size()];
}

}
