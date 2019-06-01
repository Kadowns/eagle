//
// Created by Novak on 08/05/2019.
//

#ifndef EAGLE_RENDERINGCONTEXT_H
#define EAGLE_RENDERINGCONTEXT_H

#include <string>
#include "eagle/core/Core.h"

_EAGLE_BEGIN

class RenderingContext {
public:

    RenderingContext(uint32_t width, uint32_t height);

    virtual ~RenderingContext() = default;

    virtual void init() = 0;
    virtual void refresh() = 0;
    virtual void deinit() = 0;
    virtual void set_window_handle(void* window) = 0;

    uint32_t width() const;
    uint32_t height() const;


protected:
    uint32_t m_width, m_height;

};

_EAGLE_END

#endif //EAGLE_RENDERINGCONTEXT_H
