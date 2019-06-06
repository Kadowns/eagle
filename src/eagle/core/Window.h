//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_WINDOW_H
#define EAGLE_WINDOW_H

#include <memory>
#include <queue>

#include "Core.h"
#include "events/Event.h"


#define BIND_EVENT_FN(func) std::bind(&func, this, std::placeholders::_1)

_EAGLE_BEGIN

class RenderingContext;

class Window {

public:

    using PFN_EventCallback = std::function<void(Event&)>;

    Window(RenderingContext* renderingContext, uint32_t width, uint32_t height);

    virtual ~Window();

    virtual void init() = 0;
    virtual void deinit() = 0;
    virtual void refresh() = 0;
    virtual void handle_events() = 0;

    //Returns a unique identifier for the listner
    virtual size_t add_event_listener(PFN_EventCallback callback) = 0;
    //removes the listner based on its identifier
    virtual void remove_event_listener(size_t identifier) = 0;
    virtual void* get_native_window() = 0;

    virtual bool is_minimized() = 0;
    virtual void wait_native_events() = 0;

    uint32_t get_width();
    uint32_t get_height();

protected:
    using EventQueue = std::queue<std::unique_ptr<Event>>;
    using Listener = std::pair<size_t, PFN_EventCallback>;

    std::unique_ptr<RenderingContext> m_context;

    struct WindowData {

        WindowData(uint32_t w, uint32_t h) : width(w), height(h){}

        uint32_t width, height;
        EventQueue eventQueue;

    } m_windowData;

    std::vector<Listener> m_eventListeners;
    size_t m_listenerIdentifier;

};

_EAGLE_END

#endif //EAGLE_WINDOW_H
