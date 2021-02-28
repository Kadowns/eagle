//
// Created by Ricardo on 2/15/2021.
//

#ifndef EAGLE_ANDROID_WINDOWANDROID_H
#define EAGLE_ANDROID_WINDOWANDROID_H

#include <eagle/Window.h>

struct android_app;

namespace eagle {

class VulkanContextAndroid;

class AndroidWindow : public Window {
public:
    AndroidWindow(android_app* androidApp);
    ~AndroidWindow() override;

    uint32_t width() override;

    uint32_t height() override;

    uint32_t framebuffer_width() override;

    uint32_t framebuffer_height() override;

    float framebuffer_width_scale() override;

    float framebuffer_height_scale() override;

    void init();
    void destroy();

    void pool_events() override;

    RenderingContext *rendering_context() override;

    void *native_window() override;
    bool is_minimized() override;
    bool is_surface_ready();
    void wait_native_events() override;
    void set_cursor_shape(Cursor cursorType) override;
    void set_cursor_visible(bool visible) override;

protected:
    android_app* m_androidApp;
    std::shared_ptr<VulkanContextAndroid> m_renderingContext;
    bool m_surfaceReady;

};

}

#endif //EAGLE_ANDROID_WINDOWANDROID_H
