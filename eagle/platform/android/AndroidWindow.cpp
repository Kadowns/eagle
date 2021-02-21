//
// Created by Ricardo on 2/15/2021.
//

#include "AndroidWindow.h"
#include <android_native_app_glue.h>
#include <eagle/renderer/vulkan/platform/android/VulkanContextAndroid.h>

Eagle::AndroidWindow::AndroidWindow(android_app *androidApp) :
    m_androidApp(androidApp) {
    m_renderingContext = std::make_shared<VulkanContextAndroid>(this);
}

Eagle::AndroidWindow::~AndroidWindow() {

}

void Eagle::AndroidWindow::init() {
    if (m_surfaceReady){
        return;
    }
    if (m_renderingContext->is_initialized()){
        m_renderingContext->recreate_surface();
    }
    else {
        m_renderingContext->init();
    }
    m_surfaceReady = true;
}

void Eagle::AndroidWindow::destroy() {
    if (!m_surfaceReady){
        return;
    }
    m_renderingContext->destroy_surface();
    m_surfaceReady = false;
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
    return m_renderingContext.get();
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
    return m_surfaceReady;
}

uint32_t Eagle::AndroidWindow::width() {
    return ANativeWindow_getWidth(m_androidApp->window);
}

uint32_t Eagle::AndroidWindow::height() {
    return ANativeWindow_getHeight(m_androidApp->window);
}

uint32_t Eagle::AndroidWindow::framebuffer_width() {
    return width();
}

uint32_t Eagle::AndroidWindow::framebuffer_height() {
    return height();
}

float Eagle::AndroidWindow::framebuffer_width_scale() {
    return 1;
}

float Eagle::AndroidWindow::framebuffer_height_scale() {
    return 1;
}


