//
// Created by Novak on 05/05/2019.
//


#include "DesktopApplication.h"
#include "eagle/Time.h"
#include "eagle/Log.h"
#include "eagle/Window.h"
#include "eagle/ApplicationDelegate.h"
#include "eagle/platform/desktop/WindowGLFW.h"
#include "DesktopFileSystem.h"

namespace eagle {

DesktopApplication::DesktopApplication(uint32_t width, uint32_t height, ApplicationDelegate* delegate) {
    s_instance = this;
    m_delegate = std::shared_ptr<ApplicationDelegate>(delegate);
    m_window = std::make_shared<WindowGLFW>(width, height);
    DesktopFileSystem::init();
}

void DesktopApplication::run() {

    m_window->init(&m_eventBus);
    m_delegate->init();

    while(!m_quit){
        m_window->pool_events();
        m_delegate->step();
    }

    m_delegate->destroy();
    m_window->destroy();
}

Window &DesktopApplication::window() {
    return *m_window;
}

}
