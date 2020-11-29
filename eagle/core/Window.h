//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_WINDOW_H
#define EAGLE_WINDOW_H

#include <memory>
#include <queue>

#include "CoreGlobalDefinitions.h"
#include "events/Event.h"


#define BIND_EVENT_FN(func) std::bind(&func, this, std::placeholders::_1)

EG_BEGIN

class Window {

public:

    using PFN_EventCallback = std::function<void(Reference<Event>)>;

    Window(uint32_t width, uint32_t height);

    virtual ~Window();

    virtual void init() = 0;
    virtual void deinit() = 0;

    virtual void pool_events() = 0;

    virtual void* get_native_window() = 0;

    virtual void set_event_callback(PFN_EventCallback callback) = 0;

    virtual bool is_minimized() = 0;
    virtual void wait_native_events() = 0;

    virtual void set_cursor_shape(Cursor cursorType) = 0;
    virtual void set_cursor_visible(bool visible) = 0;

    uint32_t get_width();
    uint32_t get_height();

protected:

    struct WindowData {

        WindowData(uint32_t w, uint32_t h) : width(w), height(h){}

        uint32_t width, height;
        PFN_EventCallback eventCallback;

    } m_windowData;
};

EG_END

#endif //EAGLE_WINDOW_H
