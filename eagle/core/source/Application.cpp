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

Application::Application(const ApplicationCreateInfo& config) :
    m_window(config.windowType){
    EAGLE_SET_INFO(EAGLE_APP_NAME, config.appName);

    m_instance = this;

    Log::init(config.coreLogLevel, config.clientLogLevel);
    m_layerStack.emplace(config.layers);
}

void Application::handle_event(std::shared_ptr<Event> e){

    EventDispatcher dispatcher(*e);
    if (dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::window_close_event))){
        return;
    }
    m_eventQueue.emplace(e);

}

void Application::dispatch_events() {
    while (!m_eventQueue.empty()){
        auto& e = m_eventQueue.front();
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

    m_layerStack.init();

    while(!m_shouldClose){
        m_window->handle_events();

        dispatch_events();

        for (auto& layer : m_layerStack){
            layer->handle_update();
        }

        if (!m_window->begin_draw()){
            //failed to present image, skip all rendering for this frame
            continue;
        }

        for (auto layer = m_layerStack.end(); layer != m_layerStack.begin();){
            (*--layer)->handle_draw();
        }
        m_window->end_draw();

        m_window->refresh();
    }

    m_layerStack.deinit();

    m_window->deinit();
    EG_CORE_TRACE("Application terminated!");

}

void Application::layer_emplace_back(std::shared_ptr<Layer> layer) {
    m_layerStack.emplace_back(layer);
}

void Application::layer_emplace_front(std::shared_ptr<Layer> layer) {
    m_layerStack.emplace_front(layer);
}

void Application::layer_pop(std::shared_ptr<Layer> layer) {
    m_layerStack.pop_layer(layer);
}

bool Application::window_close_event(WindowCloseEvent& e) {
    m_shouldClose = true;
    return false;
}

void Application::layer_emplace(std::vector<std::shared_ptr<Layer>> layers) {
    m_layerStack.emplace(layers);
}

float Application::get_window_aspect() {
    return (float)get_window_width() / get_window_height();
}

uint32_t Application::get_window_width() {
    return m_instance->m_window->get_width();
}

uint32_t Application::get_window_height() {
    return m_instance->m_window->get_height();
}

void Application::event_emplace_back(std::shared_ptr<Event> e) {
    m_eventQueue.emplace(e);
}

_EAGLE_END