//
// Created by Ricardo on 04/01/21.
//

#include "compute_application.h"

#include <eagle/application.h>
#include <eagle/window.h>

TriangleApplication::TriangleApplication() {
    EG_LOG_CREATE("triangle");
    EG_LOG_PATTERN("[%T.%e] [%n] [%^%l%$] [%s:%#::%!()] %v");
    EG_LOG_LEVEL(spdlog::level::info);
}

void TriangleApplication::init() {
    EG_INFO("triangle", "Triangle attached!");
    m_renderContext = eagle::Application::instance().window().render_context();

    m_listener.attach(&eagle::Application::instance().event_bus());
    m_listener.subscribe<eagle::OnWindowClose>([](const eagle::OnWindowClose& ev){
        eagle::Application::instance().quit();
        return false;
    });

    eagle::VertexLayout vertexLayout{};
    vertexLayout.add(0, eagle::Format::R32G32_SFLOAT);
    vertexLayout.add(0, eagle::Format::R32G32B32A32_SFLOAT);

    eagle::ShaderStageDescription vertexShaderDescription = {};
    vertexShaderDescription.stage = eagle::ShaderStage::VERTEX;
    vertexShaderDescription.spirVCode = eagle::Shader::read_spir_v_code("color.vert.spv");

    eagle::ShaderStageDescription fragmentShaderDescription = {};
    vertexShaderDescription.stage = eagle::ShaderStage::FRAGMENT;
    vertexShaderDescription.spirVCode = eagle::Shader::read_spir_v_code("color.frag.spv");

    eagle::ShaderCreateInfo pipelineInfo = {};
    pipelineInfo.shaderStages = {
            vertexShaderDescription, fragmentShaderDescription
    };
    pipelineInfo.renderPass = m_renderContext->main_render_pass();
    pipelineInfo.vertexLayout = vertexLayout;
    m_shader = m_renderContext->create_shader(pipelineInfo);

    struct Vertex {
        float position[2];
        float color[4];
    };

    Vertex vertices[4];
    vertices[0] = {{-1.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}};
    vertices[1] = {{1.0f,  1.0f}, {0.0f, 1.0f, 1.0f, 1.0f}};
    vertices[2] = {{1.0f, -1.0f}, {1.0f, 0.0f, 1.0f, 1.0f}};
    vertices[3] = {{-1.0f, -1.0f}, {1.0f, 0.0f, 1.0f, 1.0f}};

    eagle::VertexBufferCreateInfo vbCreateInfo = {};
    vbCreateInfo.updateType = eagle::UpdateType::BAKED;

    m_vertexBuffer = m_renderContext->create_vertex_buffer(vbCreateInfo, vertices, 4 * sizeof(Vertex));

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

    m_indexBuffer = m_renderContext->create_index_buffer(ibCreateInfo, indices, 6 * sizeof(uint16_t));

    m_color = {1.0f, 0.2f, 0.3f, 1.0f};
    m_uniformBuffer = m_renderContext->create_uniform_buffer(sizeof(Color), m_color.data);


    eagle::DescriptorBindingDescription bindingDescription = {};
    bindingDescription.size = sizeof(Color);
    bindingDescription.descriptorType = eagle::DescriptorType::UNIFORM_BUFFER;
    bindingDescription.shaderStage = eagle::ShaderStage::FRAGMENT;

    eagle::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.bindings = {bindingDescription};

    m_descriptorSetLayout = m_renderContext->create_descriptor_set_layout(descriptorSetLayoutCreateInfo);

    eagle::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
    descriptorSetCreateInfo.descriptors = {m_uniformBuffer.get()};
    descriptorSetCreateInfo.layout = m_descriptorSetLayout.get();

    m_descriptorSet = m_renderContext->create_descriptor_set(descriptorSetCreateInfo);

    eagle::CommandBufferCreateInfo commandBufferCreateInfo = {};
    commandBufferCreateInfo.level = eagle::CommandBufferLevel::MASTER;
    m_commandBuffer = m_renderContext->create_command_buffer(commandBufferCreateInfo);

    m_framesInFlight = m_renderContext->create_fence();
    m_renderFinished = m_renderContext->create_semaphore();
    m_frameAvailable = m_renderContext->create_semaphore();

    m_timer.start();
}

void TriangleApplication::step() {

    m_timer.update();

    auto time = m_timer.time();

    m_color.r = abs(sinf(time));
    m_color.g = abs(cosf(time * 0.5f));
    m_color.b = abs(acosf(time * 0.25f));

    m_uniformBuffer->write(m_color);
    m_uniformBuffer->flush();

    m_framesInFlight->wait();

    if (!m_renderContext->prepare_frame(m_frameAvailable.get())){
        EG_WARNING("triangle", "Failed to prepare frame, skipping");
        return;
    }

    m_commandBuffer->begin();
    m_commandBuffer->begin_render_pass(m_renderContext->main_render_pass(), m_renderContext->main_framebuffer());
    m_commandBuffer->bind_shader(m_shader.get());
    m_commandBuffer->bind_descriptor_sets(m_descriptorSet.get(), 0);
    m_commandBuffer->bind_vertex_buffer(m_vertexBuffer.get(), 0);
    m_commandBuffer->bind_index_buffer(m_indexBuffer.get());
    m_commandBuffer->draw_indexed(6, 1, 0, 0, 0);
    m_commandBuffer->end_render_pass();
    m_commandBuffer->end();


    eagle::CommandBufferSubmitInfo submitInfo = {};

    //command buffers to execute
    eagle::CommandBuffer* commandBuffers[] = {m_commandBuffer.get()};
    submitInfo.commandBuffers = commandBuffers;

    //fences to synchronize with cpu
    submitInfo.fence = m_framesInFlight.get();

    //semaphores to wait for signal before starting
    //wait for color output
    eagle::PipelineStageFlags waitStages[] = {eagle::PipelineStageFlagsBits::COLOR_ATTACHMENT_OUTPUT_BIT};
    eagle::Semaphore* waitSemaphores[] = {m_frameAvailable.get()};
    submitInfo.waitStages = waitStages;
    submitInfo.waitSemaphores = waitSemaphores;

    //semaphores to signal when finished
    eagle::Semaphore* signalSemaphores[] = {m_renderFinished.get()};
    submitInfo.signalSemaphores = signalSemaphores;

    submitInfo.queueType = eagle::QueueType::GRAPHICS;

    m_renderContext->submit(submitInfo);

    m_renderContext->present_frame(signalSemaphores);
}

TriangleApplication::~TriangleApplication() {
    m_listener.destroy();
}

