//
// Created by Ricardo on 11/28/2020.
//

#include "TriangleLayer.h"


void TriangleLayer::handle_attach(Eagle::EventBus* eventBus) {
    EG_INFO("Triangle attached!");
    m_renderingContext = std::make_shared<Eagle::VulkanContext>();
    m_renderingContext->init(&Eagle::Application::instance().window());

    m_listener.attach(eventBus);
    m_listener.subscribe<Eagle::OnWindowClose>(this);
    m_listener.subscribe<Eagle::OnWindowResized>(this);

    Eagle::VertexLayout vertexLayout(3, {Eagle::Format::R32G32_SFLOAT, Eagle::Format::R32G32B32_SFLOAT});

    Eagle::ShaderCreateInfo pipelineInfo = {m_renderingContext->main_render_pass(), {
            {Eagle::ShaderStage::VERTEX, "data/color.vert"},
            {Eagle::ShaderStage::FRAGMENT, "data/color.frag"}
    }};
    pipelineInfo.vertexLayout = vertexLayout;
    m_shader = m_renderingContext->create_shader(pipelineInfo);

    std::vector<float> vertices = {
            -0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
            0.0f,  -0.5f, 0.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 1.0f, 0.0f, 1.0f
    };


    m_vertexBuffer = m_renderingContext->create_vertex_buffer(vertices.data(), vertices.size(), vertexLayout, Eagle::BufferUsage::CONSTANT);
}

void TriangleLayer::handle_detach() {
    EG_INFO("Triangle deattached!");
    m_listener.detach();

    m_renderingContext->deinit();
    m_renderingContext.reset();
}

void TriangleLayer::handle_update() {

    if (!m_renderingContext->prepare_frame()){
        EG_WARNING("Failed to prepare frame, skipping");
        return;
    }

    auto commandBuffer = m_renderingContext->create_command_buffer();
    commandBuffer->begin();
    commandBuffer->begin_render_pass(m_renderingContext->main_render_pass(), m_renderingContext->main_frambuffer());
    commandBuffer->bind_shader(m_shader.lock());
    commandBuffer->bind_vertex_buffer(m_vertexBuffer.lock());
    commandBuffer->draw(3);
    commandBuffer->end_render_pass();
    commandBuffer->finish();
    commandBuffer->submit();

    m_renderingContext->present_frame();
}

bool TriangleLayer::receive(const Eagle::OnWindowClose &e) {
    Eagle::Application::instance().quit();
    return false;
}

bool TriangleLayer::receive(const Eagle::OnWindowResized &e) {
    m_renderingContext->handle_window_resized(e.width, e.height);
    return false;
}

