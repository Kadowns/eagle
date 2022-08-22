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

    m_renderQueue = m_renderingContext->create_queue({eagle::GPUQueueUsage::RENDER});

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

    m_framesInFlight->wait();

    if (!m_renderingContext->prepare_frame(m_frameAvailable.get())){
        EG_WARNING("triangle", "Failed to prepare frame, skipping");
        return;
    }

    m_commandBuffer->begin();
    m_commandBuffer->begin_render_pass(m_renderingContext->main_render_pass(), m_renderingContext->main_framebuffer());
    m_commandBuffer->bind_shader(m_shader);
    m_commandBuffer->bind_vertex_buffer(m_vertexBuffer, 0);
    m_commandBuffer->bind_index_buffer(m_indexBuffer);
    m_commandBuffer->draw_indexed(6, 1, 0, 0, 0);
    m_commandBuffer->end_render_pass();
    m_commandBuffer->end();


    eagle::GPUQueueSubmitInfo submitInfo = {};

    //command buffers to execute
    eagle::CommandBuffer* commandBuffers[] = {m_commandBuffer.get()};
    submitInfo.commandBuffers = commandBuffers;

    //fences to synchronize with cpu
    submitInfo.fence = m_framesInFlight.get();

    //semaphores to wait for signal before starting
    eagle::Semaphore* waitSemaphores[] = {m_frameAvailable.get()};
    submitInfo.waitSemaphores = waitSemaphores;

    //semaphores to signal when finished
    eagle::Semaphore* signalSemaphores[] = {m_renderFinished.get()};
    submitInfo.signalSemaphores = signalSemaphores;

    //wait for color output
    eagle::PipelineStageFlagsBits waitStages[] = {eagle::PipelineStageFlagsBits::COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitStages = waitStages;

    m_renderQueue->submit(submitInfo);

    m_renderingContext->present_frame();
}

TriangleApplication::~TriangleApplication() {
    EG_INFO("triangle", "Triangle destroyed!");
    m_listener.destroy();
}

