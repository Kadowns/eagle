//
// Created by Novak on 01/11/2019.
//

#include <eagle/engine/Renderer.h>

#include <eagle/engine/components/Transform.h>
#include <eagle/engine/components/Renderable.h>

EG_ENGINE_BEGIN

Renderer* Renderer::s_instance = nullptr;

Renderer& Renderer::instance() {
    return *s_instance;
}

void Renderer::initialize(Window &window) {
    s_instance = new Renderer(window);
}

void Renderer::terminate() {
    delete s_instance;
}

Renderer::Renderer(Window &window) {
    m_renderingContext = std::make_shared<VulkanContext>();
    m_renderingContext->init(&window);
}

Renderer::~Renderer() {
    m_renderingContext->deinit();
}



EG_ENGINE_END
