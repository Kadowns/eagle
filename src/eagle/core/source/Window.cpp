//
// Created by Novak on 05/05/2019.
//


#include "eagle/core/Window.h"
#include "eagle/renderer/RenderingContext.h"

_EAGLE_BEGIN

Window::Window(RenderingContext* renderingContext) :
    m_context(renderingContext){
}

Window::~Window() = default;



uint32_t Window::width() {
    return m_context->width();
}

uint32_t Window::height() {
    return m_context->height();
}

_EAGLE_END
