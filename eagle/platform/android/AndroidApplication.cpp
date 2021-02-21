//
// Created by Ricardo on 2/15/2021.
//

#include <android_native_app_glue.h>
#include "AndroidApplication.h"
#include "AndroidWindow.h"
#include "AndroidFileSystem.h"
#include <eagle/ApplicationDelegate.h>

void Eagle::AndroidApplication::handle_app_cmd(android_app *pApp, int32_t cmd) {
    auto self = (AndroidApplication*)pApp->userData;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            self->m_window->create_surface();
            break;
        case APP_CMD_TERM_WINDOW:
            self->m_window->destroy_surface();
            break;
        case APP_CMD_STOP:
            self->quit();
            break;
        default:
            break;
    }
}


Eagle::AndroidApplication::AndroidApplication(android_app *androidApp, ApplicationDelegate *delegate) {
    s_instance = this;
    androidApp->onAppCmd = handle_app_cmd;
    androidApp->userData = this;
    m_delegate = Reference<ApplicationDelegate>(delegate);
    m_window = std::make_shared<AndroidWindow>(androidApp);
    m_androidApp = androidApp;
    AndroidFileSystem::init(androidApp->activity->assetManager);
}

void Eagle::AndroidApplication::run() {

    bool initializedDelegate = false;
    while (!m_quit){
        m_window->pool_events();
        if (!m_window->is_surface_ready()){
            continue;

        }
        if (!initializedDelegate){
            m_delegate->init();
            initializedDelegate = true;
        }
        m_delegate->step();
    }
    m_delegate->destroy();
}

Eagle::Window& Eagle::AndroidApplication::window() {
    return *m_window.get();
}
