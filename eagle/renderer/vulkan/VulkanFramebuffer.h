//
// Created by Novak on 08/08/2020.
//

#ifndef EAGLE_VULKANFRAMEBUFFER_H
#define EAGLE_VULKANFRAMEBUFFER_H

#include <eagle/renderer/vulkan/VulkanCore.h>
#include <eagle/renderer/vulkan/VulkanImage.h>
#include <eagle/renderer/Framebuffer.h>
#include "VulkanRenderPass.h"

EG_BEGIN

struct VulkanFramebufferCreateInfo {
    VkDevice device;
    uint32_t imageCount;
};


class VulkanFramebuffer : public Framebuffer {
public:
    VulkanFramebuffer(const FramebufferCreateInfo& createInfo,
                      const VulkanFramebufferCreateInfo& nativeFramebufferCreateInfo);
    virtual ~VulkanFramebuffer();

    inline const std::vector<VkFramebuffer>& native_framebuffers() const { return m_framebuffers; }
private:
    void create_framebuffer();
private:
    VulkanFramebufferCreateInfo m_nativeCreateInfo;
    std::vector<Reference<VulkanImage>> m_nativeImageAttachments;
    std::vector<VkFramebuffer> m_framebuffers;
};

EG_END

#endif //EAGLE_VULKANFRAMEBUFFER_H
