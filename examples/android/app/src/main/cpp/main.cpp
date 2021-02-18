//
// Created by Ricardo on 2/15/2021.
//

#include <android_native_app_glue.h>
#include <jni.h>
#include <iostream>

#include "vulkan/vulkan.h"
#include "vulkan/vulkan_android.h"

extern "C" {
void handle_cmd(android_app *pApp, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:

            break;

        case APP_CMD_TERM_WINDOW:

            break;
        case APP_CMD_DESTROY:

            break;
    }
}

void android_main(struct android_app *pApp) {
    pApp->onAppCmd = handle_cmd;

    int events;
    android_poll_source *pSource;
    do {
        if (ALooper_pollAll(0, nullptr, &events, (void **) &pSource) >= 0) {
            if (pSource) {
                pSource->process(pApp, pSource);
            }
        }
    } while (!pApp->destroyRequested);
}
}