//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_WINDOW_H
#define EAGLE_WINDOW_H

#include <eagle/core_global_definitions.h>
#include <eagle/events/event.h>

namespace eagle {

class RenderContext;

struct WindowCreateInfo {
    const char* applicationName;
    const char* engineName;
};

class Window {
public:
    explicit Window(const WindowCreateInfo& createInfo);
    virtual ~Window() = default;

    virtual void pool_events() = 0;

    virtual RenderContext* render_context() = 0;

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
protected:
    WindowCreateInfo m_createInfo;
};

}

#endif //EAGLE_WINDOW_H
