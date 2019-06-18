//
// Created by Novak on 12/05/2019.
//

#include "eagle/core/WindowGLFW.h"
#include "eagle/core/events/WindowEvents.h"
#include "eagle/core/events/InputEvents.h"
#include "eagle/renderer/RenderingContext.h"

#include <GLFW/glfw3.h>
#include <eagle/core/Log.h>

_EAGLE_BEGIN

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

    m_window = glfwCreateWindow(m_windowData.width, m_windowData.height, ("Eagle - " + EAGLE_GET_INFO(EAGLE_APP_NAME)).c_str(), nullptr, nullptr);

    if (!m_window){
        glfwTerminate();
        throw std::runtime_error("Failed to create glfw window!");
    }

    glfwMakeContextCurrent(m_window);

    m_windowData.context->init(this);

    EG_CORE_TRACE("Setting up GLFW callbacks!");

    glfwSetWindowUserPointer(m_window, &m_windowData);

    //window resize
    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height){
        auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.width = width;
        data.height = height;
        data.eventCallback(std::make_shared<WindowResizedEvent>(width, height));
        data.context->handle_window_resized(width, height);
    });

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
        data.mouseX = static_cast<uint32_t>(x);
        data.mouseY = static_cast<uint32_t>(y);
        data.eventCallback(std::make_shared<MouseMoveEvent>(x, y));
    });

    //mouse click
    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int key, int action, int mod){
        auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
        if (action == GLFW_PRESS){
            data.eventCallback(std::make_shared<MousePressedEvent>(data.mouseX, data.mouseY, key));
        }else{
            data.eventCallback(std::make_shared<MouseReleasedEvent>(data.mouseX, data.mouseY, key));
        }
    });

    EG_CORE_TRACE("Window initialized!");
}

void WindowGLFW::deinit() {
    m_windowData.context->deinit();
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
    return m_windowData.width == 0 || m_windowData.height == 0;
}

void WindowGLFW::wait_native_events() {
    glfwWaitEvents();
}

void WindowGLFW::begin_draw() {
    m_windowData.context->begin_draw();
}

void WindowGLFW::end_draw() {
    m_windowData.context->end_draw();
}

void WindowGLFW::set_event_callback(Window::PFN_EventCallback callback) {
    m_windowData.eventCallback = callback;
}


_EAGLE_END
