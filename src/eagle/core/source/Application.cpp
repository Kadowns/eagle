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
    Log::init(config.coreLogLevel, config.clientLogLevel);

    m_layerStack.emplace(config.layers);
}

void Application::handle_event(Event& e){

    EventDispatcher dispatcher(e);
    dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::window_close_event));

    for (auto& layer : m_layerStack){
        layer->handle_event(e);
        if (e.is_handled())
            break;
    }
}

void Application::run() {

    EG_CORE_TRACE_F("Initializing {0}", EAGLE_GET_INFO(EAGLE_APP_NAME));

    m_window->init();
    size_t identifier = m_window->add_event_listener(BIND_EVENT_FN(Application::handle_event));

    m_layerStack.init();

    while(!m_shouldClose){
        m_window->handle_events();

        for (auto& layer : m_layerStack){
            layer->handle_update();
        }

        m_window->begin_draw();
        for (auto& layer : m_layerStack){
            layer->handle_draw();
        }
        m_window->end_draw();

        m_window->refresh();
    }

    m_layerStack.deinit();

    m_window->remove_event_listener(identifier);

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

_EAGLE_END