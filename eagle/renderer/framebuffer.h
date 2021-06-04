//
// Created by Novak on 02/08/2020.
//

#ifndef EAGLE_FRAMEBUFFER_H
#define EAGLE_FRAMEBUFFER_H

#include <eagle/renderer/renderer_global_definitions.h>
#include <eagle/renderer/image.h>
#include <eagle/renderer/render_pass.h>

namespace eagle {

struct FramebufferCreateInfo {
    WeakPointer<RenderPass> renderPass;
    std::vector<WeakPointer<Image>> attachments;
    uint32_t width, height;
};

class Framebuffer {
public:
    Framebuffer(const FramebufferCreateInfo& createInfo) : m_createInfo(createInfo) {}
    virtual ~Framebuffer() = default;

    inline const std::vector<WeakPointer<Image>>& attachments() const { return m_createInfo.attachments; }
    inline uint32_t width() const { return m_createInfo.width; }
    inline uint32_t height() const { return m_createInfo.height; }

protected:
    FramebufferCreateInfo m_createInfo;
};

}

#endif //EAGLE_FRAMEBUFFER_H
