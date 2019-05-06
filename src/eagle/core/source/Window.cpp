//
// Created by Novak on 05/05/2019.
//

#include "../Window.h"
#include <GLFW/glfw3.h>


_EAGLE_BEGIN

Window::Window(const std::string &name, int width, int height) :
        m_name(name),
        m_width(width),
        m_height(height){

}

Window::~Window() = default;

void Window::init() {

    if (!glfwInit()){
        return;
    }

    m_window = glfwCreateWindow(m_width, m_height, m_name.c_str(), nullptr, nullptr);

    if (!m_window){
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);

}

void Window::refresh() {
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void Window::deinit() {
    glfwTerminate();
}

bool Window::should_close() {
    return glfwWindowShouldClose(m_window);
}



_EAGLE_END
