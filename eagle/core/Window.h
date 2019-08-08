//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_WINDOW_H
#define EAGLE_WINDOW_H

#include <memory>
#include <queue>

#include "Core.h"
#include "events/Event.h"
#include "eagle/renderer/RenderingContext.h"


#define BIND_EVENT_FN(func) std::bind(&func, this, std::placeholders::_1)

_EAGLE_BEGIN

class Window {

public:

    using PFN_EventCallback = std::function<void(std::shared_ptr<Event>)>;

    Window(RenderingContext* renderingContext, uint32_t width, uint32_t height);

    virtual ~Window();

    virtual void init() = 0;
    virtual void deinit() = 0;

    virtual void refresh() = 0;
    virtual void handle_events() = 0;

    virtual void* get_native_window() = 0;

    virtual void set_event_callback(PFN_EventCallback callback) = 0;

    virtual bool is_minimized() = 0;
    virtual void wait_native_events() = 0;

    virtual void set_cursor_shape(EG_CURSOR cursorType) = 0;


    std::shared_ptr<RenderingContext> get_rendering_context();
    uint32_t get_width();
    uint32_t get_height();

protected:

    struct WindowData {

        WindowData(uint32_t w, uint32_t h, RenderingContext* renderingContext) : width(w), height(h), context(renderingContext){}

        uint32_t width, height;
        PFN_EventCallback eventCallback;
        std::shared_ptr<RenderingContext> context;

    } m_windowData;
};

_EAGLE_END

#endif //EAGLE_WINDOW_H
