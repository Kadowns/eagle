//
// Created by Novak on 08/05/2019.
//

#ifndef EAGLE_RENDERINGCONTEXT_H
#define EAGLE_RENDERINGCONTEXT_H

#include "eagle/core/Core.h"

_EAGLE_BEGIN

class Window;

class RenderingContext {
public:

    RenderingContext() = default;

    virtual ~RenderingContext() = default;
    virtual void init(Window *window) = 0;
    virtual void refresh() = 0;
    virtual void deinit() = 0;

};

_EAGLE_END

#endif //EAGLE_RENDERINGCONTEXT_H
