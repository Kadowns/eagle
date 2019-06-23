//
// Created by Novak on 23/06/2019.
//

#ifndef EAGLE_RENDERTARGET_H
#define EAGLE_RENDERTARGET_H

#include "eagle/core/Core.h"
#include "Image.h"
#include <memory>

_EAGLE_BEGIN

class RenderTarget{
public:
    virtual std::weak_ptr<Image> get_image() = 0;
    virtual uint32_t get_width() = 0;
    virtual uint32_t get_height() = 0;

    virtual ~RenderTarget() = default;
    RenderTarget() = default;
};

_EAGLE_END

#endif //EAGLE_RENDERTARGET_H
