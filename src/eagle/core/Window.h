//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_WINDOW_H
#define EAGLE_WINDOW_H

#include "Core.h"

_EAGLE_BEGIN

class RenderingContext;

class Window {

public:

    Window(RenderingContext* renderingContext);

    ~Window();

    virtual void init() = 0;
    virtual void deinit() = 0;
    virtual void refresh() = 0;
    virtual bool should_close() = 0;

    uint32_t width();
    uint32_t height();

protected:

    RenderingContext* m_context;
};

_EAGLE_END

#endif //EAGLE_WINDOW_H
