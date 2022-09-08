//
// Created by Novak on 08/08/2020.
//

#ifndef EAGLE_VULKANFRAMEBUFFER_H
#define EAGLE_VULKANFRAMEBUFFER_H

#include <eagle/renderer/vulkan/vulkan_global_definitions.h>
#include <eagle/renderer/vulkan/vulkan_image.h>
#include <eagle/renderer/framebuffer.h>
#include "vulkan_render_pass.h"

namespace eagle {

struct VulkanFramebufferCreateInfo {
    VkDevice device;
    uint32_t frameCount;
    uint32_t* currentFrame;
};


class VulkanFramebuffer : public Framebuffer {
public:
    VulkanFramebuffer(const FramebufferCreateInfo& createInfo,
                      const VulkanFramebufferCreateInfo& nativeFramebufferCreateInfo);
    virtual ~VulkanFramebuffer();

    VkFramebuffer current_native_framebuffer();

private:
    void create_framebuffer();
private:
    VulkanFramebufferCreateInfo m_nativeCreateInfo;
    std::vector<VkFramebuffer> m_framebuffers;
};

}

#endif //EAGLE_VULKANFRAMEBUFFER_H
