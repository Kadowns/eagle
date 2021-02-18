//
// Created by Ricardo on 2/15/2021.
//

#include <android_native_app_glue.h>
#include "AndroidApplication.h"
#include "AndroidWindow.h"

void Eagle::AndroidApplication::handle_app_cmd(android_app *pApp, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:

            break;
        case APP_CMD_TERM_WINDOW:

            break;
        case APP_CMD_DESTROY:

            break;
    }
}


Eagle::AndroidApplication::AndroidApplication(android_app *androidApp,
                                              ApplicationDelegate *delegate) {
    androidApp->onAppCmd = handle_app_cmd;
    androidApp->userData = this;
    m_delegate = Reference<ApplicationDelegate>(delegate);
    m_window = std::make_shared<AndroidWindow>(androidApp);
    m_androidApp = androidApp;
}

void Eagle::AndroidApplication::run() {

    m_window->init(&m_eventBus);
    while (!m_quit){
        m_window->pool_events();

    }
}



