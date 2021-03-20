//
// Created by Ricardo on 04/01/21.
//

#include "triangle_application.h"

#include <eagle/application.h>
#include <eagle/window.h>

TriangleApplication::TriangleApplication() : m_stackAllocator(1024 * 1024 * 10) {
    EG_LOG_CREATE("triangle");
    EG_LOG_PATTERN("[%T.%e] [%n] [%^%l%$] [%s:%#::%!()] %v");
    EG_LOG_LEVEL(spdlog::level::trace);
}

void TriangleApplication::init() {
    EG_INFO("triangle", "Triangle attached!");
    m_renderingContext = eagle::Application::instance().window().rendering_context();

    m_listener.attach(&eagle::Application::instance().event_bus());
    m_listener.subscribe<eagle::OnWindowClose>([](const eagle::OnWindowClose& ev){
        eagle::Application::instance().quit();
        return false;
    });


    m_listener.subscribe(m_renderingContext->context_recreated, [this](eagle::RenderingContext* context){
        bake_command_buffers();
        return false;
    });


    eagle::VertexLayout vertexLayout({eagle::Format::R32G32_SFLOAT, eagle::Format::R32G32B32A32_SFLOAT});

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

    auto stackScope = m_stackAllocator.scope();

    auto vertices = stackScope.construct<Vertex>(4);
    vertices[0] = {{-0.5f, 0.5f}, {1.0f, 1.0f, 0.0f, 1.0f}};
    vertices[1] = {{0.5f,  -0.5f}, {0.0f, 1.0f, 1.0f, 1.0f}};
    vertices[2] = {{-0.7f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}};
    vertices[3] = {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 0.0f}};

    m_vertexBuffer = m_renderingContext->create_vertex_buffer(vertices, 4 * sizeof(Vertex), vertexLayout, eagle::UpdateType::BAKED);

    auto indices = stackScope.construct<uint16_t>(6);
    indices[0] = 0;
    indices[1] = 2;
    indices[2] = 1;

    indices[3] = 0;
    indices[4] = 1;
    indices[5] = 3;

    m_indexBuffer = m_renderingContext->create_index_buffer(indices, 6 * sizeof(uint16_t), eagle::IndexBufferType::UINT_16, eagle::UpdateType::BAKED);

    eagle::CommandBufferCreateInfo commandBufferCreateInfo = {};
    commandBufferCreateInfo.level = eagle::CommandBufferLevel::PRIMARY;
    m_primaryCommandBuffer = m_renderingContext->create_command_buffer(commandBufferCreateInfo);

    commandBufferCreateInfo.level = eagle::CommandBufferLevel::SECONDARY;
    commandBufferCreateInfo.updateType = eagle::UpdateType::BAKED;
    m_secondaryCommandBuffer = m_renderingContext->create_command_buffer(commandBufferCreateInfo);

    bake_command_buffers();
}

void TriangleApplication::step() {
    if (!m_renderingContext->prepare_frame()){
        EG_WARNING("triangle", "Failed to prepare frame, skipping");
        return;
    }

    auto primaryCommandBuffer = m_primaryCommandBuffer.lock();

    primaryCommandBuffer->begin();
    primaryCommandBuffer->begin_render_pass(m_renderingContext->main_render_pass(), m_renderingContext->main_frambuffer());
    primaryCommandBuffer->execute_commands({m_secondaryCommandBuffer.lock()});
    primaryCommandBuffer->end_render_pass();
    primaryCommandBuffer->end();
    m_renderingContext->present_frame(primaryCommandBuffer);
}

void TriangleApplication::destroy() {
    EG_INFO("triangle", "Triangle destroyed!");
    m_listener.destroy();
}

void TriangleApplication::bake_command_buffers() {
    auto secondaryCommandBuffer = m_secondaryCommandBuffer.lock();
    secondaryCommandBuffer->begin(m_renderingContext->main_render_pass(), m_renderingContext->main_frambuffer());
    secondaryCommandBuffer->bind_shader(m_shader.lock());
    secondaryCommandBuffer->bind_vertex_buffer(m_vertexBuffer.lock());
    secondaryCommandBuffer->bind_index_buffer(m_indexBuffer.lock());
    secondaryCommandBuffer->draw_indexed(6, 0, 0);
    secondaryCommandBuffer->end();
}

