//
// Created by Novak on 05/05/2019.
//


#include "../Window.h"

#include "eagle/renderer/VulkanContext.h"

_EAGLE_BEGIN

Window::Window(const std::string &name, int width, int height) :
    m_name(name) {
    m_context = new VulkanContext(width, height);
}

Window::~Window() = default;

void Window::init() {

    m_context->init(m_name);
}

void Window::refresh() {
    m_context->refresh();
}

void Window::deinit() {
    m_context->deinit();
}

bool Window::should_close() {
    return m_context->should_terminate_context();
}

uint32_t Window::width() {
    return m_context->width();
}

uint32_t Window::height() {
    return m_context->height();
}

_EAGLE_END
