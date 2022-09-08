//
// Created by Novak on 02/08/2020.
//

#ifndef EAGLE_FRAMEBUFFER_H
#define EAGLE_FRAMEBUFFER_H

#include <eagle/renderer/renderer_global_definitions.h>
#include <eagle/renderer/image.h>
#include <eagle/renderer/render_pass.h>

#include <utility>

namespace eagle {

struct FramebufferCreateInfo {
    std::vector<std::shared_ptr<Image>> attachments;
    RenderPass* renderPass;
    uint32_t width, height;
};

class Framebuffer {
public:
    Framebuffer(FramebufferCreateInfo createInfo) : m_createInfo(std::move(createInfo)) {}
    virtual ~Framebuffer() = default;

    inline uint32_t width() const { return m_createInfo.width; }
    inline uint32_t height() const { return m_createInfo.height; }

protected:
    FramebufferCreateInfo m_createInfo;
};

}

#endif //EAGLE_FRAMEBUFFER_H
