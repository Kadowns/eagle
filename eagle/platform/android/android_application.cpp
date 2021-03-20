//
// Created by Ricardo on 2/15/2021.
//

#include <android_native_app_glue.h>
#include "android_application.h"
#include "android_window.h"
#include "android_file_system.h"
#include <eagle/application_delegate.h>

void eagle::AndroidApplication::handle_app_cmd(android_app *pApp, int32_t cmd) {
    auto self = (AndroidApplication*)pApp->userData;
    switch (cmd) {
        case APP_CMD_INPUT_CHANGED:
            break;
        case APP_CMD_INIT_WINDOW:
            self->m_window->init();
            break;
        case APP_CMD_TERM_WINDOW:
            self->m_window->destroy();
            break;
        case APP_CMD_WINDOW_RESIZED:
            break;
        case APP_CMD_WINDOW_REDRAW_NEEDED:
            break;
        case APP_CMD_CONTENT_RECT_CHANGED:
            break;
        case APP_CMD_GAINED_FOCUS:
            break;
        case APP_CMD_LOST_FOCUS:
            break;
        case APP_CMD_CONFIG_CHANGED:
            break;
        case APP_CMD_LOW_MEMORY:
            break;
        case APP_CMD_START:
            break;
        case APP_CMD_RESUME:
            break;
        case APP_CMD_SAVE_STATE:
            break;
        case APP_CMD_PAUSE:
            break;
        case APP_CMD_STOP:
            break;
        case APP_CMD_DESTROY:
            self->quit();
            break;
        default:
            break;
    }
}

void eagle::AndroidApplication::handle_input(android_app *app, AInputEvent *event) {
    int32_t type = AInputEvent_getType(event);

}


eagle::AndroidApplication::AndroidApplication(android_app *androidApp, ApplicationDelegate *delegate) {
    s_instance = this;
    androidApp->onAppCmd = handle_app_cmd;
    androidApp->userData = this;
    m_delegate = std::shared_ptr<ApplicationDelegate>(delegate);
    m_window = std::make_shared<AndroidWindow>(androidApp);
    m_androidApp = androidApp;
    AndroidFileSystem::init(androidApp->activity->assetManager);
}

void eagle::AndroidApplication::run() {

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
    m_window->destroy();
}

eagle::Window& eagle::AndroidApplication::window() {
    return *(m_window.get());
}


