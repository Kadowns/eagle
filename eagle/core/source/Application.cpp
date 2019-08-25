//
// Created by Novak on 05/05/2019.
//


#include "eagle/core/Log.h"
#include "eagle/core/Application.h"
#include "eagle/core/Window.h"
#include "eagle/core/events/InputEvents.h"
#include "eagle/core/events/WindowEvents.h"

EG_BEGIN

Application* Application::m_instance = nullptr;

Application& Application::instance() {
    return *m_instance;
}

Application::Application(const ApplicationCreateInfo& config) :
    m_window(config.windowType){
    EAGLE_SET_INFO(EAGLE_APP_NAME, config.appName);

    m_instance = this;

    Log::init(config.coreLogLevel, config.clientLogLevel);
    m_layerStack.emplace(config.layers);
}

void Application::handle_event(Reference<Event> e){
    m_eventQueue.emplace(e);
}

void Application::dispatch_events() {
    while (!m_eventQueue.empty()){
        auto& e = m_eventQueue.front();
        m_dispatcher.dispatch(*e);
        for (auto& layer : m_layerStack){
            layer->handle_event(*e);
            if ((*e).is_handled())
                break;
        }
        m_eventQueue.pop();
    }
}


void Application::run() {

    EG_CORE_TRACE_F("Initializing {0}", EAGLE_GET_INFO(EAGLE_APP_NAME));

    m_window->init();
    m_window->set_event_callback(BIND_EVENT_FN(Application::handle_event));

    m_dispatcher.add_listener<WindowCloseEvent>([&](Event& e){
        return window_close_event(*(WindowCloseEvent*)&e);
    });

    m_dispatcher.add_listener<MouseMoveEvent>([&](Event& e){
        return mouse_move_event(*(MouseMoveEvent*)&e);
    });

    m_dispatcher.add_listener<MouseButtonEvent>([&](Event& e){
        return mouse_button_event(*(MouseButtonEvent*)&e);
    });

    m_dispatcher.add_listener<MouseScrolledEvent>([&](Event& e){
        return mouse_scroll_event(*(MouseScrolledEvent*)&e);
    });

    m_dispatcher.add_listener<KeyEvent>([&](Event& e){
        return key_event(*(KeyEvent*)&e);
    });

    m_layerStack.init();

    auto renderingContext = m_window->get_rendering_context();

    while(!m_shouldClose){
        m_window->handle_events();

        dispatch_events();

        for (auto& layer : m_layerStack){
            layer->handle_update();
        }
        Input::instance().refresh();

        if (!renderingContext->begin_draw_commands()){
            //failed to present image, skip all rendering for this frame
            continue;
        }

        for (auto layer = m_layerStack.end(); layer != m_layerStack.begin();){
            (*--layer)->handle_draw();
        }
        renderingContext->end_draw_commands();

        m_window->refresh();
    }

    m_layerStack.deinit();

    m_window->deinit();
    EG_CORE_TRACE("Application terminated!");

}

void Application::layer_emplace_back(Reference<Layer> layer) {
    m_layerStack.emplace_back(layer);
}

void Application::layer_emplace_front(Reference<Layer> layer) {
    m_layerStack.emplace_front(layer);
}

void Application::layer_pop(Reference<Layer> layer) {
    m_layerStack.pop_layer(layer);
}

bool Application::window_close_event(WindowCloseEvent& e) {
    m_shouldClose = true;
    return false;
}

void Application::layer_emplace(std::vector<Reference<Layer>> layers) {
    m_layerStack.emplace(layers);
}

void Application::event_emplace_back(Reference<Event> e) {
    m_eventQueue.emplace(e);
}

bool Application::key_event(KeyEvent &e) {
    Input::instance().handle_key(e);
    return false;
}

bool Application::mouse_button_event(MouseButtonEvent &e) {
    Input::instance().handle_mouse_button(e);
    return false;
}

bool Application::mouse_scroll_event(MouseScrolledEvent &e) {
    Input::instance().handle_mouse_scroll(e);
    return false;
}

bool Application::mouse_move_event(MouseMoveEvent &e) {
    Input::instance().handle_mouse_move(e);
    return false;
}

EG_END