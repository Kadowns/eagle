//
// Created by Novak on 05/05/2019.
//


#include "eagle/core/Window.h"
#include "eagle/renderer/RenderingContext.h"

_EAGLE_BEGIN

Window::Window(RenderingContext* renderingContext, uint32_t width, uint32_t height) :
    m_context(renderingContext),
    m_width(width),
    m_height(height){
}

Window::~Window() = default;



uint32_t Window::get_width() {
    return m_width;
}

uint32_t Window::get_height() {
    return m_height;
}

_EAGLE_END
