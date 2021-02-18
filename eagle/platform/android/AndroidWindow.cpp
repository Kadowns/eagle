//
// Created by Ricardo on 2/15/2021.
//

#include "AndroidWindow.h"
#include <android_native_app_glue.h>
#include <eagle/renderer/vulkan/platform/android/VulkanContextAndroid.h>

Eagle::AndroidWindow::AndroidWindow(android_app *androidApp) :
    m_androidApp(androidApp) {
    m_renderingContext = std::make_shared<VulkanContextAndroid>();
}

Eagle::AndroidWindow::~AndroidWindow() {

}

void Eagle::AndroidWindow::on_surface_created() {

}

void Eagle::AndroidWindow::on_surface_destroyed() {

}

void Eagle::AndroidWindow::pool_events() {
    int events;
    android_poll_source *pSource;
    if (ALooper_pollAll(0, nullptr, &events, (void **) &pSource) >= 0) {
        if (pSource) {
            pSource->process(m_androidApp, pSource);
        }
    }
}

Eagle::RenderingContext *Eagle::AndroidWindow::rendering_context() {
    return nullptr;
}

void *Eagle::AndroidWindow::native_window() {
    return m_androidApp->window;
}

bool Eagle::AndroidWindow::is_minimized() {
    return false;
}

void Eagle::AndroidWindow::wait_native_events() {

}

void Eagle::AndroidWindow::set_cursor_shape(Eagle::Cursor cursorType) {

}

void Eagle::AndroidWindow::set_cursor_visible(bool visible) {

}

bool Eagle::AndroidWindow::is_surface_ready() {

}


