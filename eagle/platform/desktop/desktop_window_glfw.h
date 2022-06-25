//
// Created by Novak on 12/05/2019.
//

#ifndef EAGLE_WINDOWGLFW_H
#define EAGLE_WINDOWGLFW_H


#include <eagle/core_global_definitions.h>
#include <eagle/window.h>
#include <eagle/memory/pointer.h>

struct GLFWwindow;
struct GLFWcursor;

namespace eagle {

class VulkanContextGLFW;

class DesktopWindowGLFW : public Window {
public:

    DesktopWindowGLFW(uint32_t width, uint32_t height);
    ~DesktopWindowGLFW();

    void init(EventBus* eventBus);

    void pool_events() override;

    RenderingContext* rendering_context() override;

    void* native_window() override;

    bool is_minimized() override;
    void wait_native_events() override;

    void set_cursor_shape(Cursor cursorType) override;
    void set_cursor_visible(bool visible) override;

    uint32_t width() override;
    uint32_t height() override;

    uint32_t framebuffer_width() override;
    uint32_t framebuffer_height() override;

    float framebuffer_width_scale() override;
    float framebuffer_height_scale() override;

private:
    struct WindowData {
        WindowData(uint32_t w, uint32_t h) : width(w), height(h){}

        uint32_t width, height;
        uint32_t framebufferWidth, framebufferHeight;
        EventBus* eventBus;
    } m_windowData;

    GLFWwindow* m_window;
    std::map<Cursor, GLFWcursor*> m_mouseCursors;
    StrongPointer<VulkanContextGLFW> m_renderingContext;
};

}


#endif //EAGLE_WINDOWGLFW_H
