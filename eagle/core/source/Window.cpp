//
// Created by Novak on 05/05/2019.
//

#include "eagle/core/Window.h"

EG_BEGIN

Window::Window(uint32_t width, uint32_t height) :
    m_windowData(width, height) {

}

Window::~Window() = default;



uint32_t Window::get_width() {
    return m_windowData.width;
}

uint32_t Window::get_height() {
    return m_windowData.height;
}

EG_END
