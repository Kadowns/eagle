//
// Created by Novak on 05/05/2019.
//


#include "eagle/core/Log.h"
#include "eagle/core/Application.h"
#include "../Window.h"
#include "../DefaultWindow.h"

_EAGLE_BEGIN

Application* Application::m_instance = nullptr;

Application& Application::instance() {
    return *m_instance;
}

Application::Application(const AppConfig& config) :
    m_window(config.windowType){
    EAGLE_SET_INFO(EAGLE_APP_NAME, config.appName);
}

void Application::run() {

    Log::init();
    EG_TRACE_F("Initializing application {0}", EAGLE_GET_INFO(EAGLE_APP_NAME));

    m_window->init();


    while(!m_window->should_close()){
        m_window->refresh();
    }

    m_window->deinit();

}

_EAGLE_END