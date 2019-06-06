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
    m_window(config.windowType),
    m_layerStack(config.layers){
    EAGLE_SET_INFO(EAGLE_APP_NAME, config.appName);
    Log::init(config.coreLogLevel, config.clientLogLevel);
}

void Application::handle_event(Event& e){

    if (e.get_event_type() == WindowCloseEvent::get_static_type()){
        m_shouldClose = true;
    }

    for (auto& layer : m_layerStack){
        layer->handle_event(e);
        if (e.Handled)
            break;
    }
}

void Application::run() {

    EG_CORE_TRACE_F("Initializing {0}", EAGLE_GET_INFO(EAGLE_APP_NAME));

    m_window->init();
    size_t identifier = m_window->add_event_listener(BIND_EVENT_FN(Application::handle_event));

    while(!m_shouldClose){
        m_window->handle_events();

        for (auto& layer : m_layerStack){
            layer->handle_update();
        }

        m_window->refresh();
    }

    m_window->remove_event_listener(identifier);

    m_window->deinit();

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

_EAGLE_END