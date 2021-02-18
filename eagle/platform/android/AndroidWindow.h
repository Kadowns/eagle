//
// Created by Ricardo on 2/15/2021.
//

#ifndef EAGLE_ANDROID_WINDOWANDROID_H
#define EAGLE_ANDROID_WINDOWANDROID_H

#include <eagle/Window.h>

struct android_app;

EG_BEGIN

class AndroidWindow : public Window {
public:
    AndroidWindow(android_app* androidApp);
    ~AndroidWindow() override;

    void on_surface_created();
    void on_surface_destroyed();

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
    Reference<RenderingContext> m_renderingContext;

};

EG_END

#endif //EAGLE_ANDROID_WINDOWANDROID_H
