//
// Created by Novak on 23/06/2019.
//

#ifndef EAGLE_RENDERTARGET_H
#define EAGLE_RENDERTARGET_H

#include "eagle/core_global_definitions.h"
#include "image.h"

namespace eagle {


enum class RenderTargetAttachment {
    COLOR
};

class RenderTarget {
public:
    virtual ~RenderTarget() = default;

    virtual std::weak_ptr<Image> get_image() = 0;
};

}

#endif //EAGLE_RENDERTARGET_H
