//
// Created by Ricardo on 04/01/21.
//

#include "TriangleApp.h"

#include <eagle/Application.h>
#include <eagle/Window.h>

TriangleApp::TriangleApp() {

}

void TriangleApp::init() {
    EG_INFO("Triangle attached!");
    m_renderingContext = eagle::Application::instance().window().rendering_context();

    m_listener.attach(&eagle::Application::instance().event_bus());
    m_listener.subscribe<eagle::OnWindowClose>([](const eagle::OnWindowClose& ev){
        eagle::Application::instance().quit();
        return false;
    });

    eagle::VertexLayout vertexLayout(5, {eagle::Format::R32G32_SFLOAT, eagle::Format::R32G32B32_SFLOAT});

    eagle::ShaderCreateInfo pipelineInfo = {m_renderingContext->main_render_pass(), {
            {eagle::ShaderStage::VERTEX, "color.vert.spv"},
            {eagle::ShaderStage::FRAGMENT, "color.frag.spv"}
    }};
    pipelineInfo.vertexLayout = vertexLayout;
    m_shader = m_renderingContext->create_shader(pipelineInfo);

    struct Vertex {
        float position[2];
        float color[3];
    };

    std::vector<Vertex> vertices = {
            Vertex{{-0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}},
            Vertex{{0.5f,  -0.5f}, {0.0f, 1.0f, 1.0f}},
            Vertex{{-0.7f, -0.5f}, {1.0f, 0.0f, 1.0f}},
            Vertex{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}
    };


    m_vertexBuffer = m_renderingContext->create_vertex_buffer(vertices.data(), vertices.size() * sizeof(Vertex), vertexLayout, eagle::UpdateType::CONSTANT);


    std::vector<uint16_t> indices = {
            0, 2, 1,
            0, 1, 3
    };

    m_indexBuffer = m_renderingContext->create_index_buffer(indices.data(), indices.size() * sizeof(uint16_t), eagle::IndexBufferType::UINT_16, eagle::UpdateType::CONSTANT);

}

void TriangleApp::step() {
    if (!m_renderingContext->prepare_frame()){
        EG_WARNING("Failed to prepare frame, skipping");
        return;
    }

    auto commandBuffer = m_renderingContext->main_command_buffer();
    commandBuffer->begin();
    commandBuffer->begin_render_pass(m_renderingContext->main_render_pass(), m_renderingContext->main_frambuffer());
    commandBuffer->bind_shader(m_shader.lock());
    commandBuffer->bind_vertex_buffer(m_vertexBuffer.lock());
    commandBuffer->bind_index_buffer(m_indexBuffer.lock());
    commandBuffer->draw_indexed(6, 0, 0);
    commandBuffer->end_render_pass();
    commandBuffer->finish();

    m_renderingContext->submit_command_buffer(commandBuffer);
    m_renderingContext->present_frame(<#initializer#>);
}

void TriangleApp::destroy() {
    EG_INFO("Triangle deattached!");
    m_listener.detach();
}

