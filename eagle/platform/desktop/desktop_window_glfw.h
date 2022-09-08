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

class VulkanRenderContextGLFW;

struct DesktopWindowGLFWCreateInfo {
    uint32_t width;
    uint32_t height;
};

class DesktopWindowGLFW : public Window {
public:

    DesktopWindowGLFW(const WindowCreateInfo& createInfo, DesktopWindowGLFWCreateInfo desktopWindowCreateInfo);
    ~DesktopWindowGLFW() override;

    void init(EventBus* eventBus);

    void pool_events() override;

    RenderContext* render_context() override;

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
    DesktopWindowGLFWCreateInfo m_desktopCreateInfo;
    uint32_t m_framebufferWidth, m_framebufferHeight;
    EventBus* m_eventBus;
    GLFWwindow* m_glfwWindow;
    std::map<Cursor, GLFWcursor*> m_mouseCursors;
    std::shared_ptr<VulkanRenderContextGLFW> m_renderContext;
};

}


#endif //EAGLE_WINDOWGLFW_H
