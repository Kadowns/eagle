//
// Created by Ricardo on 2/15/2021.
//

#include "android_window.h"
#include <android_native_app_glue.h>
#include <eagle/renderer/vulkan/platform/android/vulkan_context_android.h>

eagle::AndroidWindow::AndroidWindow(android_app *androidApp) :
    m_androidApp(androidApp) {
    m_renderingContext = std::make_shared<VulkanContextAndroid>(this);
}

eagle::AndroidWindow::~AndroidWindow() {

}

void eagle::AndroidWindow::init() {
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

void eagle::AndroidWindow::destroy() {
    if (!m_surfaceReady){
        return;
    }
    m_renderingContext->destroy_surface();
    m_surfaceReady = false;
}

void eagle::AndroidWindow::pool_events() {
    int events;
    android_poll_source *pSource;
    if (ALooper_pollAll(0, nullptr, &events, (void **) &pSource) >= 0) {
        if (pSource) {
            pSource->process(m_androidApp, pSource);
        }
    }
}

eagle::RenderingContext *eagle::AndroidWindow::rendering_context() {
    return m_renderingContext.get();
}

void *eagle::AndroidWindow::native_window() {
    return m_androidApp->window;
}

bool eagle::AndroidWindow::is_minimized() {
    return false;
}

void eagle::AndroidWindow::wait_native_events() {

}

void eagle::AndroidWindow::set_cursor_shape(eagle::Cursor cursorType) {

}

void eagle::AndroidWindow::set_cursor_visible(bool visible) {

}

bool eagle::AndroidWindow::is_surface_ready() {
    return m_surfaceReady;
}

uint32_t eagle::AndroidWindow::width() {
    return ANativeWindow_getWidth(m_androidApp->window);
}

uint32_t eagle::AndroidWindow::height() {
    return ANativeWindow_getHeight(m_androidApp->window);
}

uint32_t eagle::AndroidWindow::framebuffer_width() {
    return width();
}

uint32_t eagle::AndroidWindow::framebuffer_height() {
    return height();
}

float eagle::AndroidWindow::framebuffer_width_scale() {
    return 1;
}

float eagle::AndroidWindow::framebuffer_height_scale() {
    return 1;
}


