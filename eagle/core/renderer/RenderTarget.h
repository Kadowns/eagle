//
// Created by Novak on 23/06/2019.
//

#ifndef EAGLE_RENDERTARGET_H
#define EAGLE_RENDERTARGET_H

#include "eagle/core/Core.h"
#include "Image.h"

EG_BEGIN


enum class RENDER_TARGET_ATTACHMENT {
    COLOR
};

class RenderTarget {
public:
    RenderTarget(uint32_t width, uint32_t height, const std::vector<RENDER_TARGET_ATTACHMENT> &attachments):
        m_width(width), m_height(height),
        m_attachments(attachments){}
    virtual ~RenderTarget() = default;

    virtual Handle<Image> get_image(size_t index) = 0;
    virtual std::vector<Handle<Image>> get_images() = 0;

    const std::vector<RENDER_TARGET_ATTACHMENT>& get_attachments() const {return m_attachments;}

protected:
    uint32_t m_width, m_height;
    std::vector<RENDER_TARGET_ATTACHMENT> m_attachments;

};

EG_END

#endif //EAGLE_RENDERTARGET_H
