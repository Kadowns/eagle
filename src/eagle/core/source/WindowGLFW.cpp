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

    m_context->init(this);

    EG_CORE_TRACE("Setting up GLFW callbacks!");

    glfwSetWindowUserPointer(m_window, &m_windowData);

    //window resize
    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height){
        auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.width = width;
        data.height = height;
        data.eventQueue.emplace(std::make_unique<WindowResizedEvent>(width, height));
    });

    //window focus
    glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused){
        auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
        if (focused == GLFW_TRUE){
            data.eventQueue.emplace(std::make_unique<WindowFocusEvent>());
        } else{
            data.eventQueue.emplace(std::make_unique<WindowLostFocusEvent>());
        }
    });

    //window close
    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window){
        auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.eventQueue.emplace(std::make_unique<WindowCloseEvent>());
    });

    //mouse move
    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y){
        auto& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.eventQueue.emplace(std::make_unique<MouseMoveEvent>((float)x, (float)y));
    });

    EG_CORE_TRACE("Window initialized!");
}

void WindowGLFW::deinit() {
    m_context->deinit();
    glfwTerminate();
}

void WindowGLFW::refresh() {
    m_context->refresh();
}

void* WindowGLFW::get_native_window() {
    return m_window;
}

void WindowGLFW::handle_events() {
    glfwPollEvents();
    while (!m_windowData.eventQueue.empty()) {
        auto& e = m_windowData.eventQueue.front();
        for (const auto& it : m_eventListeners){
            it.second(*e);
        }
        m_windowData.eventQueue.pop();
    }
}

size_t WindowGLFW::add_event_listener(PFN_EventCallback callback) {
    EG_CORE_TRACE("Added event listener!");
    m_eventListeners.emplace_back(std::make_pair(++m_listenerIdentifier, callback));
    return m_listenerIdentifier;
}

void WindowGLFW::remove_event_listener(size_t identifier) {
    EG_CORE_TRACE("Removed event listener!");
    auto listener = std::find_if(m_eventListeners.begin(), m_eventListeners.end(),[identifier](Listener listener){
        return listener.first == identifier;
    });
    if (listener != m_eventListeners.end()){
        m_eventListeners.erase(listener);
    }
}

bool WindowGLFW::is_minimized() {
    return m_windowData.width == 0 || m_windowData.height == 0;
}

void WindowGLFW::wait_native_events() {
    glfwWaitEvents();
}

void WindowGLFW::begin_draw() {
    m_context->begin_draw();
}

void WindowGLFW::end_draw() {
    m_context->end_draw();
}


_EAGLE_END
