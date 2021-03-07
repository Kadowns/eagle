//
// Created by Ricardo on 2/15/2021.
//

#include <android_native_app_glue.h>
#include <jni.h>
#include <iostream>
#include <android/log.h>

#include <eagle/platform/android/AndroidApplication.h>
#include <TriangleApp.h>

#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "threaded_app", __VA_ARGS__))

extern "C" {
void android_main(struct android_app *pApp) {
    try {
        eagle::AndroidApplication application(pApp, new TriangleApp());
        application.run();
    }
    catch (std::exception& e){
        LOGE("exception caught!");
    }
}
}