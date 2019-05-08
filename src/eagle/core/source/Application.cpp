
//
// Created by Novak on 05/05/2019.
//

#include "eagle/core/Application.h"

#include "eagle/core/Window.h"

_EAGLE_BEGIN

Application::Application(const std::string& name, int windowWidth, int windowHeight) :
    m_window(name, windowWidth, windowHeight) {

}

void Application::run() {

    m_window.init();

    while(!m_window.should_close()){
        m_window.refresh();
    }

    m_window.deinit();

}
_EAGLE_END