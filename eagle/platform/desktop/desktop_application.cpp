//
// Created by Novak on 05/05/2019.
//


#include "desktop_application.h"
#include "eagle/timer.h"
#include "eagle/log.h"
#include "eagle/window.h"
#include "eagle/application_delegate.h"
#include "eagle/platform/desktop/desktop_window_glfw.h"
#include "desktop_file_system.h"

namespace eagle {


DesktopApplication::DesktopApplication(const ApplicationCreateInfo createInfo, const DesktopApplicationCreateInfo desktopCreateInfo) :
    Application(createInfo),
    m_desktopCreateInfo(desktopCreateInfo) {

    s_instance = this;

    WindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.applicationName = m_createInfo.applicationName;
    windowCreateInfo.engineName = m_createInfo.engineName;

    DesktopWindowGLFWCreateInfo desktopWindowGlfwCreateInfo = {};
    desktopWindowGlfwCreateInfo.width = m_desktopCreateInfo.width;
    desktopWindowGlfwCreateInfo.height = m_desktopCreateInfo.height;

    m_window = std::make_shared<DesktopWindowGLFW>(windowCreateInfo, desktopWindowGlfwCreateInfo);
    DesktopFileSystem::init();

    m_delegate = std::shared_ptr<ApplicationDelegate>(m_createInfo.delegate);
}

DesktopApplication::~DesktopApplication() {
    m_delegate.reset();
    m_window.reset();
}

void DesktopApplication::run() {

    m_window->init(&m_eventBus);
    m_delegate->init();

    while(!m_quit){
        m_window->pool_events();
        m_delegate->step();
    }
}

Window &DesktopApplication::window() {
    return *m_window;
}

}
