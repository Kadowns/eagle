//
// Created by Novak on 12/05/2019.
//

#include "eagle/core/WindowGLFW.h"
#include "eagle/renderer/RenderingContext.h"

#include <GLFW/glfw3.h>
#include <eagle/core/Log.h>

_EAGLE_BEGIN

WindowGLFW::WindowGLFW(RenderingContext* context, uint32_t width, uint32_t height) :
    Window(context, width, height) {

}

void WindowGLFW::init() {

    EG_TRACE("Initializing glfw window!");

    if (!glfwInit()){
        throw std::runtime_error("Failed to initialize glfw!");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(m_width, m_height, ("Eagle - " + EAGLE_GET_INFO(EAGLE_APP_NAME)).c_str(), nullptr, nullptr);

    if (!m_window){
        glfwTerminate();
        throw std::runtime_error("Failed to create glfw window!");
    }

    glfwMakeContextCurrent(m_window);

    m_context->set_window(this);
    m_context->init();
    EG_TRACE("Window initialized!");
}

void WindowGLFW::deinit() {
    m_context->deinit();
    glfwTerminate();
}

void WindowGLFW::refresh() {
    glfwPollEvents();
    m_context->refresh();
}

bool WindowGLFW::should_close() {
    return glfwWindowShouldClose(m_window);
}

void *WindowGLFW::get_window_handle() {
    return m_window;
}


_EAGLE_END
