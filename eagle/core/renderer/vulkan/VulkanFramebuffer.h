//
// Created by Novak on 08/08/2020.
//

#ifndef EAGLE_VULKANFRAMEBUFFER_H
#define EAGLE_VULKANFRAMEBUFFER_H

#include <eagle/core/renderer/vulkan/VulkanCore.h>
#include <eagle/core/renderer/vulkan/VulkanImage.h>
#include <eagle/core/renderer/Framebuffer.h>
#include "VulkanRenderPass.h"

EG_BEGIN

struct VulkanFramebufferCreateInfo {
    VkDevice device;
};


class VulkanFramebuffer : public Framebuffer {
public:
    VulkanFramebuffer(const FramebufferCreateInfo& createInfo,
                      const VulkanFramebufferCreateInfo& nativeFramebufferCreateInfo);
    virtual ~VulkanFramebuffer();

    inline const VkFramebuffer& native_framebuffer() const { return m_framebuffer; }
private:
    void create_framebuffer();
private:
    VulkanFramebufferCreateInfo m_nativeCreateInfo;
    std::vector<Reference<VulkanImage>> m_nativeImageAttachments;
    VkFramebuffer m_framebuffer;
};

EG_END

#endif //EAGLE_VULKANFRAMEBUFFER_H
