//
// Created by Novak on 23/06/2019.
//

#ifndef EAGLE_RENDERTARGET_H
#define EAGLE_RENDERTARGET_H

#include "eagle/core/CoreGlobalDefinitions.h"
#include "Image.h"

EG_BEGIN


enum class RenderTargetAttachment {
    COLOR
};

class RenderTarget {
public:
    virtual ~RenderTarget() = default;

    virtual Handle <Image> get_image() = 0;
};

EG_END

#endif //EAGLE_RENDERTARGET_H
