//
// Created by Novak on 12/05/2019.
//

#include <eagle/platform/desktop/desktop_window_glfw.h>
#include <eagle/log.h>
#include <eagle/events/window_events.h>
#include <eagle/events/input_events.h>

#include <GLFW/glfw3.h>
#include <eagle/renderer/vulkan/platform/desktop/vulkan_render_context_glfw.h>

namespace eagle {

DesktopWindowGLFW::DesktopWindowGLFW(const WindowCreateInfo& createInfo, DesktopWindowGLFWCreateInfo desktopCreateInfo) :
    Window(createInfo),
    m_desktopCreateInfo(desktopCreateInfo){

}

DesktopWindowGLFW::~DesktopWindowGLFW() {
    if (m_renderContext){
        m_renderContext->exit();
    }

    for (auto& it : m_mouseCursors){
        glfwDestroyCursor(it.second);
    }

    m_mouseCursors.clear();

    glfwTerminate();
}

void DesktopWindowGLFW::init(EventBus* eventBus) {
    EG_LOG_CREATE("eagle");

    EG_TRACE("eagle","Initializing glfw window!");

    if (!glfwInit()){
        throw std::runtime_error("Failed to initialize glfw!");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED , GLFW_TRUE);

    m_glfwWindow = glfwCreateWindow(
            (int32_t)m_desktopCreateInfo.width,
            (int32_t)m_desktopCreateInfo.height,
            m_createInfo.applicationName,
            nullptr,
            nullptr
            );

    if (!m_glfwWindow){
        glfwTerminate();
        throw std::runtime_error("Failed to create glfw window!");
    }

    m_eventBus = eventBus;

    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(m_glfwWindow, &framebufferWidth, &framebufferHeight);
    m_framebufferWidth = framebufferWidth;
    m_framebufferHeight = framebufferHeight;

    glfwMakeContextCurrent(m_glfwWindow);
    glfwShowWindow(m_glfwWindow);

    EG_TRACE("eagle","Setting up GLFW callbacks!");

    glfwSetWindowUserPointer(m_glfwWindow, this);

    //window resize
    glfwSetWindowSizeCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, int width, int height){
        auto window = (DesktopWindowGLFW*)glfwGetWindowUserPointer(glfwWindow);

        window->m_desktopCreateInfo.width = width;
        window->m_desktopCreateInfo.height = height;

        int framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize(glfwWindow, &framebufferWidth, &framebufferHeight);
        window->m_framebufferWidth = framebufferWidth;
        window->m_framebufferHeight = framebufferHeight;

        if (width != 0 && height != 0){
            window->m_eventBus->emit(OnWindowResized{static_cast<uint32_t>(width), static_cast<uint32_t>(height)});
        }
    });

    //window focus
    glfwSetWindowFocusCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, int focused){
        auto window = (DesktopWindowGLFW*)glfwGetWindowUserPointer(glfwWindow);
        if (focused == GLFW_TRUE){
            window->m_eventBus->emit(OnWindowFocus{});
        }
        else {
            window->m_eventBus->emit(OnWindowLostFocus{});
        }
    });

    //window close
    glfwSetWindowCloseCallback(m_glfwWindow, [](GLFWwindow* glfwWindow){
        auto window = (DesktopWindowGLFW*)glfwGetWindowUserPointer(glfwWindow);
        window->m_eventBus->emit(OnWindowClose{});
    });

    //mouse move
    glfwSetCursorPosCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, double x, double y){
        auto window = (DesktopWindowGLFW*)glfwGetWindowUserPointer(glfwWindow);
        window->m_eventBus->emit(OnMouseMove{static_cast<float>(x), static_cast<float>(y)});
    });

    glfwSetScrollCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, double x, double y){
        auto window = (DesktopWindowGLFW*)glfwGetWindowUserPointer(glfwWindow);
        window->m_eventBus->emit(OnMouseScrolled{static_cast<float>(x), static_cast<float>(y)});
    });

    //mouse click
    glfwSetMouseButtonCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, int key, int action, int mod){
        auto window = (DesktopWindowGLFW*)glfwGetWindowUserPointer(glfwWindow);
        window->m_eventBus->emit(OnMouseButton{key, action, mod});
    });

    glfwSetKeyCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
        auto window = (DesktopWindowGLFW*)glfwGetWindowUserPointer(glfwWindow);
        window->m_eventBus->emit(OnKey{key, action, mods});
    });

    glfwSetCharCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, unsigned int c){
        auto window = (DesktopWindowGLFW*)glfwGetWindowUserPointer(glfwWindow);
        window->m_eventBus->emit(OnKeyTyped{c});
    });

    m_mouseCursors[Cursor::ARROW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    m_mouseCursors[Cursor::TEXT] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    m_mouseCursors[Cursor::CROSSHAIR] = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    m_mouseCursors[Cursor::HAND] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    m_mouseCursors[Cursor::HORI_RESIZE] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    m_mouseCursors[Cursor::VERT_RESIZE] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);


    RenderContextCreateInfo renderContextCreateInfo = {};
    renderContextCreateInfo.window = (Window*)this;
    renderContextCreateInfo.maxFramesInFlight = 2;
    renderContextCreateInfo.engineName = m_createInfo.engineName;
    renderContextCreateInfo.applicationName = m_createInfo.applicationName;
    renderContextCreateInfo.physicalDeviceIndex = 0;

    VulkanRenderContextCreateInfo vulkanRenderContextCreateInfo = {};
    vulkanRenderContextCreateInfo.enableValidationLayers = true;

    m_renderContext = std::make_shared<VulkanRenderContextGLFW>(renderContextCreateInfo, vulkanRenderContextCreateInfo);
    m_renderContext->init();

    EG_TRACE("eagle","Window initialized!");
}

void* DesktopWindowGLFW::native_window() {
    return m_glfwWindow;
}

void DesktopWindowGLFW::pool_events() {
    glfwPollEvents();
}

bool DesktopWindowGLFW::is_minimized() {
    int width, height;
    glfwGetWindowSize(m_glfwWindow, &width, &height);
    return width == 0 || height == 0;
}

void DesktopWindowGLFW::wait_native_events() {
    glfwWaitEvents();
}

void DesktopWindowGLFW::set_cursor_shape(Cursor cursorType) {
    auto cursor = m_mouseCursors.find(cursorType);
    glfwSetCursor(m_glfwWindow, cursor != m_mouseCursors.end() ? cursor->second : m_mouseCursors[Cursor::ARROW]);
}

void DesktopWindowGLFW::set_cursor_visible(bool visible) {
    glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

RenderContext* DesktopWindowGLFW::render_context() {
    return m_renderContext.get();
}

uint32_t DesktopWindowGLFW::width() {
    return m_desktopCreateInfo.width;
}

uint32_t DesktopWindowGLFW::height() {
    return m_desktopCreateInfo.height;
}

uint32_t DesktopWindowGLFW::framebuffer_width() {
    return m_framebufferWidth;
}

uint32_t DesktopWindowGLFW::framebuffer_height() {
    return m_framebufferHeight;
}

float DesktopWindowGLFW::framebuffer_width_scale() {
    return (float)m_framebufferWidth / (float)m_desktopCreateInfo.width;
}

float DesktopWindowGLFW::framebuffer_height_scale() {
    return (float)m_framebufferHeight / (float)m_desktopCreateInfo.height;
}

}
