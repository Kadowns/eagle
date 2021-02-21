//
// Created by Ricardo on 2/15/2021.
//

#include <android_native_app_glue.h>
#include <jni.h>
#include <iostream>

#include <eagle/platform/android/AndroidApplication.h>
#include <TriangleApp.h>

extern "C" {
void android_main(struct android_app *pApp) {
    Eagle::AndroidApplication application(pApp, new TriangleApp());
    application.run();
}
}