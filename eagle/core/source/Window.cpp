//
// Created by Novak on 05/05/2019.
//

#include "eagle/core/Window.h"

EG_BEGIN

Window::Window(uint32_t width, uint32_t height) :
    m_windowData(width, height) {

}

Window::~Window() = default;



uint32_t Window::width() {
    return m_windowData.width;
}

uint32_t Window::height() {
    return m_windowData.height;
}

uint32_t Window::framebuffer_width() {
    return m_windowData.framebufferWidth;
}

uint32_t Window::framebuffer_height() {
    return m_windowData.framebufferHeight;
}

float Window::framebuffer_width_scale() {
    return static_cast<float>(m_windowData.framebufferWidth) / m_windowData.width;
}

float Window::framebuffer_height_scale() {
    return static_cast<float>(m_windowData.framebufferHeight) / m_windowData.height;
}

EG_END
