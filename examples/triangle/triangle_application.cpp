//
// Created by Ricardo on 04/01/21.
//

#include "triangle_application.h"

#include <eagle/application.h>
#include <eagle/window.h>

TriangleApplication::TriangleApplication() {
    EG_LOG_CREATE("triangle");
    EG_LOG_PATTERN("[%T.%e] [%n] [%^%l%$] [%s:%#::%!()] %v");
    EG_LOG_LEVEL(spdlog::level::info);
}

void TriangleApplication::init() {
    EG_INFO("triangle", "Triangle attached!");
    m_renderingContext = eagle::Application::instance().window().rendering_context();

    m_listener.attach(&eagle::Application::instance().event_bus());
    m_listener.subscribe<eagle::OnWindowClose>([](const eagle::OnWindowClose& ev){
        eagle::Application::instance().quit();
        return false;
    });

    eagle::VertexLayout vertexLayout{};
    vertexLayout.add(0, eagle::Format::R32G32_SFLOAT);
    vertexLayout.add(0, eagle::Format::R32G32B32A32_SFLOAT);

    eagle::ShaderCreateInfo pipelineInfo = {m_renderingContext->main_render_pass(), {
            {eagle::ShaderStage::VERTEX, "color.vert.spv"},
            {eagle::ShaderStage::FRAGMENT, "color.frag.spv"}
    }};
    pipelineInfo.blendEnable = true;
    pipelineInfo.vertexLayout = vertexLayout;
    m_shader = m_renderingContext->create_shader(pipelineInfo);

    struct Vertex {
        float position[2];
        float color[4];
    };

    Vertex vertices[4];
    vertices[0] = {{-0.5f, 0.5f}, {1.0f, 1.0f, 0.0f, 1.0f}};
    vertices[1] = {{0.5f,  0.5f}, {0.0f, 1.0f, 1.0f, 1.0f}};
    vertices[2] = {{0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}};
    vertices[3] = {{-0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}};

    eagle::VertexBufferCreateInfo vbCreateInfo = {};
    vbCreateInfo.updateType = eagle::UpdateType::BAKED;

    m_vertexBuffer = m_renderingContext->create_vertex_buffer(vbCreateInfo, vertices, 4 * sizeof(Vertex));

    uint16_t indices[6];
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;

    eagle::IndexBufferCreateInfo ibCreateInfo = {};
    ibCreateInfo.indexType = eagle::IndexBufferType::UINT_16;
    ibCreateInfo.updateType = eagle::UpdateType::BAKED;

    m_indexBuffer = m_renderingContext->create_index_buffer(ibCreateInfo, indices, 6 * sizeof(uint16_t));

    eagle::CommandBufferCreateInfo commandBufferCreateInfo = {};
    commandBufferCreateInfo.level = eagle::CommandBufferLevel::MASTER;
    m_commandBuffer = m_renderingContext->create_command_buffer(commandBufferCreateInfo);

}

void TriangleApplication::step() {
    if (!m_renderingContext->prepare_frame()){
        EG_WARNING("triangle", "Failed to prepare frame, skipping");
        return;
    }

    m_commandBuffer->begin();
    m_commandBuffer->begin_render_pass(m_renderingContext->main_render_pass(), m_renderingContext->main_frambuffer());
    m_commandBuffer->bind_shader(m_shader);
    m_commandBuffer->bind_vertex_buffer(m_vertexBuffer, 0);
    m_commandBuffer->bind_index_buffer(m_indexBuffer);
    m_commandBuffer->draw_indexed(6, 1, 0, 0, 0);
    m_commandBuffer->end_render_pass();
    m_commandBuffer->end();

    m_renderingContext->submit_command_buffer(m_commandBuffer);
    m_renderingContext->present_frame();
}

TriangleApplication::~TriangleApplication() {
    EG_INFO("triangle", "Triangle destroyed!");
    m_listener.destroy();
}

