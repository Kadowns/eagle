//
// Created by Novak on 05/05/2019.
//


#include "eagle/core/Application.h"
#include "eagle/core/Time.h"
#include "eagle/core/Log.h"
#include "eagle/core/Window.h"

EG_BEGIN

Application* Application::m_instance = nullptr;

Application& Application::instance() {
    return *m_instance;
}

Application::Application(const std::string &appName, Reference<Window> window) : m_window(std::move(window)){
    EAGLE_SET_INFO(EAGLE_APP_NAME, appName);
    m_instance = this;
}

void Application::run() {

    EG_CORE_TRACE_F("Initializing {0}", EAGLE_GET_INFO(EAGLE_APP_NAME));

    Time::init();

    m_window->init(&m_eventBus);
    m_layerStack.init();

    while(!m_quit){

        Time::update();
        m_window->pool_events();

        for (auto& layer : m_layerStack){
            layer->handle_update();
        }
    }

    m_layerStack.deinit();

    m_window->deinit();
    EG_CORE_TRACE("Application terminated!");

}

void Application::quit() {
    m_quit = true;
}

EG_END