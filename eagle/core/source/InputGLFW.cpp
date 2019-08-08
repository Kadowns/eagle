//
// Created by Novak on 18/07/2019.
//

#include <GLFW/glfw3.h>

#include "eagle/core/InputGLFW.h"

_EAGLE_BEGIN

std::unique_ptr<Input> Input::s_instance;

bool InputGLFW::handle_key_down(int keycode) {
    auto state = glfwGetKey(m_window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool InputGLFW::handle_key_pressed(int keycode) {
    auto state = glfwGetKey(m_window, keycode);
    return state == GLFW_PRESS;
}

bool InputGLFW::handle_key_released(int keycode) {
    auto state = glfwGetKey(m_window, keycode);
    return state == GLFW_RELEASE;
}

bool InputGLFW::handle_mouse_button_down(int button) {
    auto state = glfwGetMouseButton(m_window, button);
    return state == GLFW_PRESS | state == GLFW_REPEAT;
}

bool InputGLFW::handle_mouse_button_pressed(int button) {
    auto state = glfwGetMouseButton(m_window, button);
    return state == GLFW_PRESS;
}

bool InputGLFW::handle_mouse_button_released(int button) {
    auto state = glfwGetMouseButton(m_window, button);
    return state == GLFW_RELEASE;
}

std::pair<float, float> InputGLFW::handle_mouse_position() {
    double x, y;
    glfwGetCursorPos(m_window, &x, &y);
    return { (float)x, (float)y };
}

float InputGLFW::handle_mouse_x() {
    return handle_mouse_position().first;
}

float InputGLFW::handle_mouse_y() {
    return handle_mouse_position().second;
}

InputGLFW::InputGLFW(GLFWwindow *window) :
    m_window(window){
    s_instance.reset(this);
}

InputGLFW::~InputGLFW() {

}

_EAGLE_END