//
// Created by Novak on 12/05/2019.
//

#include "WindowGLFW.h"
#include "eagle/Log.h"
#include "eagle/events/WindowEvents.h"
#include "eagle/events/InputEvents.h"


#include <GLFW/glfw3.h>
#include <eagle/renderer/vulkan/platform/desktop/VulkanContextGLFW.h>


EG_BEGIN

WindowGLFW::WindowGLFW(uint32_t width, uint32_t height) : m_windowData(width, height) {
}

void WindowGLFW::init(EventBus* eventBus) {

    EG_CORE_TRACE("Initializing glfw window!");

    if (!glfwInit()){
        throw std::runtime_error("Failed to initialize glfw!");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED , GLFW_TRUE);

    m_window = glfwCreateWindow(m_windowData.width, m_windowData.height, "Eagle - NAME_ME", nullptr, nullptr);

    if (!m_window){
        glfwTerminate();
        throw std::runtime_error("Failed to create glfw window!");
    }

    m_windowData.eventBus = eventBus;

    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(m_window, &framebufferWidth, &framebufferHeight);
    m_windowData.framebufferWidth = framebufferWidth;
    m_windowData.framebufferHeight = framebufferHeight;

    glfwMakeContextCurrent(m_window);
    glfwShowWindow(m_window);

    EG_CORE_TRACE("Setting up GLFW callbacks!");

    glfwSetWindowUserPointer(m_window, &m_windowData);

    //window resize
    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height){
        auto data = (WindowData*)glfwGetWindowUserPointer(window);
        data->width = width;
        data->height = height;

        int framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        data->framebufferWidth = framebufferWidth;
        data->framebufferHeight = framebufferHeight;

        if (width != 0 && height != 0){
            data->eventBus->emit(OnWindowResized{static_cast<uint32_t>(width), static_cast<uint32_t>(height)});
        }
    });

    //window focus
    glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused){
        auto data = (WindowData*)glfwGetWindowUserPointer(window);
        if (focused == GLFW_TRUE){
            data->eventBus->emit(OnWindowFocus{});
        }
        else {
            data->eventBus->emit(OnWindowLostFocus{});
        }
    });

    //window close
    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window){
        auto data = (WindowData*)glfwGetWindowUserPointer(window);
        data->eventBus->emit(OnWindowClose{});
    });

    //mouse move
    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y){
        auto data = (WindowData*)glfwGetWindowUserPointer(window);
        data->eventBus->emit(OnMouseMove{static_cast<float>(x), static_cast<float>(y)});
    });

    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double x, double y){
        auto data = (WindowData*)glfwGetWindowUserPointer(window);
        data->eventBus->emit(OnMouseScrolled{static_cast<float>(x), static_cast<float>(y)});
    });

    //mouse click
    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int key, int action, int mod){
        auto data = (WindowData*)glfwGetWindowUserPointer(window);
        data->eventBus->emit(OnMouseButton{key, action, mod});
    });

    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto data = (WindowData*)glfwGetWindowUserPointer(window);
        data->eventBus->emit(OnKey{key, action, mods});
    });

    glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int c){
        auto data = (WindowData*)glfwGetWindowUserPointer(window);
        data->eventBus->emit(OnKeyTyped{c});
    });

    m_mouseCursors[Cursor::ARROW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    m_mouseCursors[Cursor::TEXT] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    m_mouseCursors[Cursor::CROSSHAIR] = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    m_mouseCursors[Cursor::HAND] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    m_mouseCursors[Cursor::HORI_RESIZE] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    m_mouseCursors[Cursor::VERT_RESIZE] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);

    m_renderingContext = std::make_shared<VulkanContextGLFW>(this);
    m_renderingContext->init(eventBus);

    EG_CORE_TRACE("Window initialized!");
}

void WindowGLFW::destroy() {

    m_renderingContext->destroy();

    for (auto& it : m_mouseCursors){
        glfwDestroyCursor(it.second);
    }
    m_mouseCursors.clear();

    glfwTerminate();
}

void* WindowGLFW::native_window() {
    return m_window;
}

void WindowGLFW::pool_events() {
    glfwPollEvents();
}

bool WindowGLFW::is_minimized() {
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    return width == 0 || height == 0;
}

void WindowGLFW::wait_native_events() {
    glfwWaitEvents();
}

void WindowGLFW::set_cursor_shape(Cursor cursorType) {
    auto cursor = m_mouseCursors.find(cursorType);
    glfwSetCursor(m_window, cursor != m_mouseCursors.end() ? cursor->second : m_mouseCursors[Cursor::ARROW]);
}

void WindowGLFW::set_cursor_visible(bool visible) {
    glfwSetInputMode(m_window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

RenderingContext* WindowGLFW::rendering_context() {
    return m_renderingContext.get();
}

uint32_t WindowGLFW::width() {
    return m_windowData.width;
}

uint32_t WindowGLFW::height() {
    return m_windowData.height;
}

uint32_t WindowGLFW::framebuffer_width() {
    return m_windowData.framebufferWidth;
}

uint32_t WindowGLFW::framebuffer_height() {
    return m_windowData.framebufferHeight;
}

float WindowGLFW::framebuffer_width_scale() {
    return static_cast<float>(m_windowData.framebufferWidth) / m_windowData.width;
}

float WindowGLFW::framebuffer_height_scale() {
    return static_cast<float>(m_windowData.framebufferHeight) / m_windowData.height;
}


EG_END
