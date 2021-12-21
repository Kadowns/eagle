//
// Created by Ricardo on 10/24/2020.
//

#ifndef EAGLE_VULKANRENDERPASS_H
#define EAGLE_VULKANRENDERPASS_H

#include "vulkan_global_definitions.h"
#include <eagle/renderer/render_pass.h>

namespace eagle {

struct VulkanRenderPassCreateInfo {
    VkDevice device;
};

class VulkanRenderPass : public RenderPass {
public:
    VulkanRenderPass(RenderPassCreateInfo createInfo, const VulkanRenderPassCreateInfo& vulkanCreateInfo);

    virtual ~VulkanRenderPass();

    void cleanup();

    inline const std::vector<VkAttachmentDescription>& native_attachments() const { return m_vkAttachments; }
    inline const VkRenderPass& native_render_pass() const { return m_vkRenderPass; }
    inline const std::vector<VkClearValue>& clear_values() const { return m_clearValues; }

private:
    void create_native_render_pass();

private:
    VulkanRenderPassCreateInfo m_vulkanCreateInfo;
    std::vector<VkAttachmentDescription> m_vkAttachments;
    std::vector<VkClearValue> m_clearValues;
    VkRenderPass m_vkRenderPass;
};

}

#endif //EAGLE_VULKANRENDERPASS_H
