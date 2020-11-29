//
// Created by Ricardo on 10/24/2020.
//

#ifndef EAGLE_VULKANRENDERPASS_H
#define EAGLE_VULKANRENDERPASS_H

#include "VulkanCore.h"
#include <eagle/core/renderer/RenderPass.h>

EG_BEGIN

struct VulkanRenderPassCreateInfo {
    VkDevice device;
};


class VulkanRenderPass : public RenderPass {
public:
    VulkanRenderPass(const VulkanRenderPassCreateInfo& createInfo,
                     const VkAttachmentDescription& colorAttachment,
                     const VkAttachmentDescription& depthAttachment);


    VulkanRenderPass(const VulkanRenderPassCreateInfo& createInfo,
                     const std::vector<RenderAttachmentDescription>& colorAttachments,
                     const RenderAttachmentDescription& depthAttachment);

    VulkanRenderPass(const VulkanRenderPassCreateInfo& createInfo,
                     const std::vector<VkAttachmentDescription>& colorAttachments,
                     const VkAttachmentDescription& depthAttachment);

    virtual ~VulkanRenderPass();

    void cleanup();

    inline const std::vector<VkAttachmentDescription>& native_color_attachments() const { return m_vkColorAttachments; }
    inline const VkAttachmentDescription& native_depth_attachment() const { return m_vkDepthAttachment; }
    inline const VkRenderPass& native_render_pass() const { return m_vkRenderPass; }
    inline const std::vector<VkClearValue>& clear_values() const { return m_clearValues; }

private:
    void create_native_render_pass();

private:
    VulkanRenderPassCreateInfo m_createInfo;
    std::vector<VkAttachmentDescription> m_vkColorAttachments;
    VkAttachmentDescription m_vkDepthAttachment;
    std::vector<VkClearValue> m_clearValues;
    VkRenderPass m_vkRenderPass;
};

EG_END

#endif //EAGLE_VULKANRENDERPASS_H
