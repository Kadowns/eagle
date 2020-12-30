//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_WINDOW_H
#define EAGLE_WINDOW_H

#include "CoreGlobalDefinitions.h"
#include <eagle/core/events/EventBus.h>


EG_BEGIN

class Window {

public:

    Window(uint32_t width, uint32_t height);

    virtual ~Window();

    virtual void init() = 0;
    virtual void deinit() = 0;

    virtual void pool_events() = 0;

    virtual void* get_native_window() = 0;

    virtual bool is_minimized() = 0;
    virtual void wait_native_events() = 0;

    virtual void set_cursor_shape(Cursor cursorType) = 0;
    virtual void set_cursor_visible(bool visible) = 0;

    virtual EventBus<EventStream>* event_bus() = 0;

    uint32_t get_width();
    uint32_t get_height();

protected:

    struct WindowData {
        WindowData(uint32_t w, uint32_t h) : width(w), height(h){}

        uint32_t width, height;
        EventBus<EventStream> eventBus;
    } m_windowData;
};

EG_END

#endif //EAGLE_WINDOW_H
