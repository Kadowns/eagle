//
// Created by Novak on 05/05/2019.
//


#include "eagle/core/Window.h"
#include "eagle/renderer/RenderingContext.h"

_EAGLE_BEGIN

Window::Window(RenderingContext* renderingContext, uint32_t width, uint32_t height) :
    m_windowData(width, height, renderingContext) {

}

Window::~Window() = default;



uint32_t Window::get_width() {
    return m_windowData.width;
}

uint32_t Window::get_height() {
    return m_windowData.height;
}

std::shared_ptr<RenderingContext> Window::get_rendering_context() {
    return m_windowData.context;
}

_EAGLE_END
