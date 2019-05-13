//
// Created by Novak on 12/05/2019.
//

#include <GLFW/glfw3.h>

#include "eagle/core/DefaultWindow.h"
#include "eagle/renderer/RenderingContext.h"


_EAGLE_BEGIN

DefaultWindow::DefaultWindow(RenderingContext* context) :
    Window(context) {

}

void DefaultWindow::init() {
    if (!glfwInit()){
        return;
    }

    m_window = glfwCreateWindow(m_context->width(), m_context->height(), ("Eagle - " + EAGLE_GET_INFO(EAGLE_APP_NAME)).c_str(), nullptr, nullptr);

    if (!m_window){
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);
}

void DefaultWindow::deinit() {
    glfwTerminate();
}

void DefaultWindow::refresh() {
    m_context->refresh();
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

bool DefaultWindow::should_close() {
    return glfwWindowShouldClose(m_window);
}


_EAGLE_END
