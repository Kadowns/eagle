//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_WINDOW_H
#define EAGLE_WINDOW_H

#include "CoreGlobalDefinitions.h"
#include <eagle/events/EventBus.h>


namespace eagle {

class RenderingContext;

class Window {
public:
    virtual ~Window() = default;

    virtual void pool_events() = 0;

    virtual RenderingContext* rendering_context() = 0;

    virtual bool is_minimized() = 0;
    virtual void wait_native_events() = 0;

    virtual void* native_window() = 0;

    virtual void set_cursor_shape(Cursor cursorType) = 0;
    virtual void set_cursor_visible(bool visible) = 0;

    virtual uint32_t width() = 0;
    virtual uint32_t height() = 0;

    virtual uint32_t framebuffer_width() = 0;
    virtual uint32_t framebuffer_height() = 0;

    virtual float framebuffer_width_scale() = 0;
    virtual float framebuffer_height_scale() = 0;
};

}

#endif //EAGLE_WINDOW_H
