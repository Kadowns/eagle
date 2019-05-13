//
// Created by Novak on 05/05/2019.
//


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

    plog::init(plog::verbose, "log.txt", 0, 1);
    LOGV << "Logger initialized!";

    m_window->init();

    LOGV << "Initializing main loop!";
    while(!m_window->should_close()){
        m_window->refresh();
    }

    m_window->deinit();

}

_EAGLE_END