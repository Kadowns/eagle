//
// Created by Novak on 12/05/2019.
//

#include "eagle/core/WindowGLFW.h"
#include "eagle/core/Log.h"
#include "eagle/core/events/WindowEvents.h"
#include "eagle/core/events/InputEvents.h"
#include "eagle/core/renderer/RenderingContext.h"

#include <GLFW/glfw3.h>


EG_BEGIN

WindowGLFW::WindowGLFW(RenderingContext* context, uint32_t width, uint32_t height) :
    Window(context, width, height) {

}

void WindowGLFW::init() {

    EG_CORE_TRACE("Initializing glfw window!");

    if (!glfwInit()){
        throw std::runtime_error("Failed to initialize glfw!");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED , GLFW_TRUE);

    m_window = glfwCreateWindow(m_windowData.width, m_windowData.height, ("Eagle - " + EAGLE_GET_INFO(EAGLE_APP_NAME)).c_str(), nullptr, nullptr);

    if (!m_window){
        glfwTerminate();
        throw std::runtime_error("Failed to create glfw window!");
    }

    glfwMakeContextCurrent(m_window);
    glfwShowWindow(m_window);

    m_windowData.context->init(this);

    EG_CORE_TRACE("Setting up GLFW callbacks!");

    glfwSetWindowUserPointer(m_window, &m_windowData);

    //window resize
    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height){
        auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.width = width;
        data.height = height;
        data.context->handle_window_resized(width, height);
        if (width != 0 && height != 0){
            data.eventCallback(std::make_shared<WindowResizedEvent>(width, height));
        }
    });
    glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    //window focus
    glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused){
        auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
        if (focused == GLFW_TRUE){
            data.eventCallback(std::make_shared<WindowFocusEvent>());
        } else{
            data.eventCallback(std::make_shared<WindowLostFocusEvent>());
        }
    });

    //window close
    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window){
        auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.eventCallback(std::make_shared<WindowCloseEvent>());
    });

    //mouse move
    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y){
        auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.eventCallback(std::make_shared<MouseMoveEvent>(static_cast<float>(x), static_cast<float>(y)));
    });

    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double x, double y){
       auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
       data.eventCallback(std::make_shared<MouseScrolledEvent>(static_cast<float>(x),static_cast<float>(y)));
    });

    //mouse click
    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int key, int action, int mod){
        auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.eventCallback(std::make_shared<MouseButtonEvent>(key, action, mod));
    });

    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto &data = *(WindowData *) glfwGetWindowUserPointer(window);
        data.eventCallback(std::make_shared<KeyEvent>(key, action, mods));
    });

    glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int c){
        auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.eventCallback(std::make_shared<KeyTypedEvent>(c));
    });

    m_mouseCursors[EG_CURSOR::ARROW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    m_mouseCursors[EG_CURSOR::TEXT] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    m_mouseCursors[EG_CURSOR::CROSSHAIR] = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    m_mouseCursors[EG_CURSOR::HAND] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    m_mouseCursors[EG_CURSOR::HORI_RESIZE] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    m_mouseCursors[EG_CURSOR::VERT_RESIZE] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);

    EG_CORE_TRACE("Window initialized!");
}

void WindowGLFW::deinit() {
    m_windowData.context->deinit();

    for (auto& it : m_mouseCursors){
        glfwDestroyCursor(it.second);
    }
    m_mouseCursors.clear();

    glfwTerminate();
}

void WindowGLFW::refresh() {
    m_windowData.context->refresh();
}

void* WindowGLFW::get_native_window() {
    return m_window;
}

void WindowGLFW::handle_events() {
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

void WindowGLFW::set_event_callback(Window::PFN_EventCallback callback) {
    m_windowData.eventCallback = callback;
}

void WindowGLFW::set_cursor_shape(EG_CURSOR cursorType) {
    auto cursor = m_mouseCursors.find(cursorType);
    glfwSetCursor(m_window, cursor != m_mouseCursors.end() ? cursor->second : m_mouseCursors[EG_CURSOR::ARROW]);
}

void WindowGLFW::set_cursor_visible(bool visible) {
    glfwSetInputMode(m_window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

EG_END
