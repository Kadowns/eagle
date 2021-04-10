//
// Created by Ricardo on 04/01/21.
//

#include "triangle_application.h"

#include <eagle/application.h>
#include <eagle/window.h>

TriangleApplication::TriangleApplication() : m_stackAllocator(1024 * 1024 * 10), m_poolAllocator(10) {
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

    m_listener.receive<eagle::OnMouseMove>(this);

    m_listener.subscribe(m_renderingContext->context_recreated, [this](eagle::RenderingContext* context){
        bake_command_buffers();
        return false;
    });

//    auto transform1 = m_poolAllocator.construct();
//    auto transform2 = m_poolAllocator.construct(25.0f);
//    auto transform3 = m_poolAllocator.construct();
//    auto transform4 = m_poolAllocator.construct();
//
//    transform3->position[0] = 3506.235f;
//
//    EG_INFO("triangle", "pool size: {0}, pool capacity: {1}", m_poolAllocator.size(), m_poolAllocator.capacity());
//    m_poolAllocator.destroy(transform3);
//    EG_INFO("triangle", "pool size: {0}, pool capacity: {1}", m_poolAllocator.size(), m_poolAllocator.capacity());
//
//    auto transform5 = m_poolAllocator.construct();
//
//    auto transform6 = std::move(transform5);

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

    eagle::VertexBufferCreateInfo vbCreateInfo = {};
    vbCreateInfo.updateType = eagle::UpdateType::BAKED;
    vbCreateInfo.size = 4 * sizeof(Vertex);
    vbCreateInfo.data = vertices;

    m_vertexBuffer = m_renderingContext->create_vertex_buffer(vbCreateInfo);

    auto indices = stackScope.construct<uint16_t>(6);
    indices[0] = 0;
    indices[1] = 2;
    indices[2] = 1;

    indices[3] = 0;
    indices[4] = 1;
    indices[5] = 3;

    m_indexBuffer = m_renderingContext->create_index_buffer({eagle::UpdateType::BAKED, eagle::IndexBufferType::UINT_16, 6 * sizeof(uint16_t), indices});

    eagle::CommandBufferCreateInfo commandBufferCreateInfo = {};
    commandBufferCreateInfo.level = eagle::CommandBufferLevel::PRIMARY;
    m_primaryCommandBuffer = m_renderingContext->create_command_buffer(commandBufferCreateInfo);

    commandBufferCreateInfo.level = eagle::CommandBufferLevel::SECONDARY;
    m_secondaryCommandBuffer = m_renderingContext->create_command_buffer(commandBufferCreateInfo);

}

void TriangleApplication::step() {
    if (!m_renderingContext->prepare_frame()){
        EG_WARNING("triangle", "Failed to prepare frame, skipping");
        return;
    }

    bake_command_buffers();

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
    EG_INFO("triangle", "pool size: {0}, pool capacity: {1}", m_poolAllocator.size(), m_poolAllocator.capacity());
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

bool TriangleApplication::receive(const eagle::OnMouseMove& ev) {
    EG_INFO("triangle", "Mouse on position {0}, {1}", ev.x, ev.y);
    return false;
}

