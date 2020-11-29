//
// Created by Novak on 02/08/2020.
//

#ifndef EAGLE_FRAMEBUFFER_H
#define EAGLE_FRAMEBUFFER_H

#include <eagle/core/renderer/RenderingCore.h>
#include <eagle/core/renderer/Image.h>
#include <eagle/core/renderer/RenderPass.h>

EG_BEGIN

struct FramebufferCreateInfo {
    Reference<RenderPass> renderPass;
    std::vector<Reference<Image>> attachments;
    uint32_t width, height;
};

class Framebuffer {
public:
    Framebuffer(const FramebufferCreateInfo& createInfo) : m_createInfo(createInfo) {}
    virtual ~Framebuffer() = default;

    inline const std::vector<Reference<Image>>& attachments() const { return m_createInfo.attachments; }
    inline uint32_t width() const { return m_createInfo.width; }
    inline uint32_t height() const { return m_createInfo.height; }

protected:
    FramebufferCreateInfo m_createInfo;
};

EG_END

#endif //EAGLE_FRAMEBUFFER_H
