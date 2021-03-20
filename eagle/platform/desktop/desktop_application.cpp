//
// Created by Novak on 05/05/2019.
//


#include "desktop_application.h"
#include "eagle/time.h"
#include "eagle/log.h"
#include "eagle/window.h"
#include "eagle/application_delegate.h"
#include "eagle/platform/desktop/desktop_window_glfw.h"
#include "desktop_file_system.h"

namespace eagle {

DesktopApplication::DesktopApplication(uint32_t width, uint32_t height, ApplicationDelegate* delegate) {
    s_instance = this;
    m_delegate = std::shared_ptr<ApplicationDelegate>(delegate);
    m_window = std::make_shared<DesktopWindowGLFW>(width, height);
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
