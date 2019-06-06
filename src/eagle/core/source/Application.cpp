//
// Created by Novak on 05/05/2019.
//


#include "eagle/core/Log.h"
#include "eagle/core/Application.h"
#include "eagle/core/Window.h"
#include "eagle/core/events/InputEvents.h"
#include "eagle/core/events/WindowEvents.h"

_EAGLE_BEGIN

Application* Application::m_instance = nullptr;

Application& Application::instance() {
    return *m_instance;
}

Application::Application(const AppConfig& config) :
    m_window(config.windowType),
    m_shouldClose(false){
    EAGLE_SET_INFO(EAGLE_APP_NAME, config.appName);
}

void Application::handle_event(Event& e){
    switch(e.get_event_type()){
        case EVENT_TYPE::WINDOW_CLOSE: m_shouldClose = true; break;
        case EVENT_TYPE::WINDOW_RESIZE:break;
        case EVENT_TYPE::WINDOW_FOCUS: EG_TRACE("Window focused!"); break;
        case EVENT_TYPE::WINDOW_LOST_FOCUS: EG_TRACE("Window lost focus!");break;
        case EVENT_TYPE::WINDOW_MOVED:break;
        case EVENT_TYPE::KEY_PRESSED:break;
        case EVENT_TYPE::KEY_RELEASED:break;
        case EVENT_TYPE::KEY_TYPED:break;
        case EVENT_TYPE::MOUSE_BUTTON_PRESSED:break;
        case EVENT_TYPE::MOUSE_BUTTON_RELEASED:break;
        case EVENT_TYPE::MOUSE_MOVE: {
            MouseMoveEvent& mouse = *(MouseMoveEvent*)&e;
            EG_TRACE_F("Mouse on position: x:{0}-y:{1}", mouse.get_x(), mouse.get_y());
        }
        break;

        case EVENT_TYPE::MOUSE_SCROLLED:break;
        default: throw std::runtime_error("Invalid event type!");
    }
}

void Application::run() {

    Log::init();
    EG_TRACE_F("Initializing {0}", EAGLE_GET_INFO(EAGLE_APP_NAME));

    m_window->init();
    size_t identifier = m_window->add_event_listener(BIND_EVENT_FN(Application::handle_event));

    while(!m_shouldClose){
        m_window->handle_events();
        m_window->refresh();
    }

    m_window->remove_event_listener(identifier);

    m_window->deinit();

}

_EAGLE_END