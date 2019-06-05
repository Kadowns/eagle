//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_WINDOW_H
#define EAGLE_WINDOW_H

#include "Core.h"
#include <memory>

_EAGLE_BEGIN

class RenderingContext;

class Window {

public:

    Window(RenderingContext* renderingContext, uint32_t width, uint32_t height);

    virtual ~Window();

    virtual void init() = 0;
    virtual void deinit() = 0;
    virtual void refresh() = 0;
    virtual bool should_close() = 0;

    virtual void* get_window_handle() = 0;

    uint32_t get_width();
    uint32_t get_height();

protected:

    std::unique_ptr<RenderingContext> m_context;

    uint32_t m_width, m_height;
};

_EAGLE_END

#endif //EAGLE_WINDOW_H
