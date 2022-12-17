//
// Created by Ricardo on 04/01/21.
//

#include "compute_application.h"

#include <eagle/application.h>
#include <eagle/window.h>

namespace detail {

struct ReleaseResourceInfo {
    eagle::Image* image;
    eagle::CommandBuffer* commandBuffer;
    eagle::CommandQueue* dstQueue;
    eagle::PipelineStageFlags srcPipelineStages;
    eagle::PipelineStageFlags dstPipelineStages;
};

static void transfer_queue_ownership(const ReleaseResourceInfo& releaseInfo){
    releaseInfo.commandBuffer->begin();
    eagle::ImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.image = releaseInfo.image;

    imageMemoryBarrier.srcQueue = releaseInfo.commandBuffer->command_queue();
    imageMemoryBarrier.dstQueue = releaseInfo.dstQueue;

    imageMemoryBarrier.srcAccessMask = 0;
    imageMemoryBarrier.dstAccessMask = 0;

    imageMemoryBarrier.oldLayout = releaseInfo.image->layout();
    imageMemoryBarrier.newLayout = releaseInfo.image->layout();

    eagle::ImageMemoryBarrier imageMemoryBarriers[] = {imageMemoryBarrier};

    releaseInfo.commandBuffer->pipeline_barrier(
            imageMemoryBarriers,
            eagle::PipelineStageFlagsBits::TOP_OF_PIPE_BIT,
            eagle::PipelineStageFlagsBits::COMPUTE_SHADER_BIT
    );
    releaseInfo.commandBuffer->end();
}
}


ComputeApplication::ComputeApplication() {
    EG_LOG_CREATE("compute");
    EG_LOG_PATTERN("[%T.%e] [%n] [%^%l%$] [%s:%#::%!()] %v");
    EG_LOG_LEVEL(spdlog::level::info);
}

void ComputeApplication::init() {
    EG_INFO("compute", "Triangle attached!");
    m_renderContext = eagle::Application::instance().window().render_context();

    m_listener.attach(&eagle::Application::instance().event_bus());
    m_listener.subscribe<eagle::OnWindowClose>([](const eagle::OnWindowClose& ev){
        eagle::Application::instance().quit();
        return false;
    });

    eagle::ShaderStageDescription vertexShaderDescription = {};
    vertexShaderDescription.stage = eagle::ShaderStage::VERTEX;
    vertexShaderDescription.spirVCode = eagle::Shader::read_spir_v_code("color.vert.spv");

    eagle::ShaderStageDescription fragmentShaderDescription = {};
    fragmentShaderDescription.stage = eagle::ShaderStage::FRAGMENT;
    fragmentShaderDescription.spirVCode = eagle::Shader::read_spir_v_code("color.frag.spv");

    eagle::ShaderCreateInfo graphicsPipelineInfo = {};
    graphicsPipelineInfo.shaderStages = {
            vertexShaderDescription, fragmentShaderDescription
    };
    graphicsPipelineInfo.cullMode = eagle::CullMode::NONE;
    graphicsPipelineInfo.renderPass = m_renderContext->main_render_pass();
    m_graphicsShader = m_renderContext->create_shader(graphicsPipelineInfo);


    eagle::ShaderStageDescription computeShaderDescription = {};
    computeShaderDescription.stage = eagle::ShaderStage::COMPUTE;
    computeShaderDescription.spirVCode = eagle::Shader::read_spir_v_code("compute.comp.spv");

    eagle::ShaderCreateInfo computePipelineInfo = {};
    computePipelineInfo.shaderStages = {
            computeShaderDescription
    };
    m_computeShader = m_renderContext->create_shader(computePipelineInfo);


    eagle::ImageCreateInfo computeImageCreateInfo = {};
    computeImageCreateInfo.layout = eagle::ImageLayout::SHADER_READ_ONLY_OPTIMAL;
    computeImageCreateInfo.width = 512;
    computeImageCreateInfo.height = 512;
    computeImageCreateInfo.format = eagle::Format::R8G8B8A8_SNORM;
    computeImageCreateInfo.type = eagle::ImageType::D2;
    computeImageCreateInfo.aspects = eagle::IMAGE_ASPECT_COLOR;
    computeImageCreateInfo.memoryProperties = eagle::MEMORY_PROPERTY_DEVICE_LOCAL;
    computeImageCreateInfo.tiling = eagle::ImageTiling::OPTIMAL;
    computeImageCreateInfo.usages = eagle::IMAGE_USAGE_STORAGE | eagle::IMAGE_USAGE_SAMPLED;
    computeImageCreateInfo.owningQueue = m_renderContext->command_queue(eagle::CommandQueueType::COMPUTE);

    eagle::TextureCreateInfo computeTextureCreateInfo = {};
    computeTextureCreateInfo.filter = eagle::Filter::NEAREST;
    computeTextureCreateInfo.imageCreateInfo = computeImageCreateInfo;

    m_computeTexture = m_renderContext->create_texture(computeTextureCreateInfo);

    eagle::ImageDescriptorInfo graphicsImageDescriptorInfo = {};
    graphicsImageDescriptorInfo.descriptor = m_computeTexture.get();
    graphicsImageDescriptorInfo.binding = 0;
    graphicsImageDescriptorInfo.layout = eagle::ImageLayout::SHADER_READ_ONLY_OPTIMAL;

    eagle::DescriptorSetCreateInfo descriptorSetCreateInfo = {};
    descriptorSetCreateInfo.imageDescriptors = {graphicsImageDescriptorInfo};
    descriptorSetCreateInfo.layout = m_graphicsShader->descriptor_set_layout(0);

    m_graphicsDescriptorSet = m_renderContext->create_descriptor_set(descriptorSetCreateInfo);

    eagle::ImageDescriptorInfo computeImageDescriptorInfo = {};
    computeImageDescriptorInfo.descriptor = m_computeTexture.get();
    computeImageDescriptorInfo.binding = 0;
    computeImageDescriptorInfo.layout = eagle::ImageLayout::GENERAL;

    eagle::DescriptorSetCreateInfo computeDescriptorSetInfo = {};
    computeDescriptorSetInfo.imageDescriptors = {computeImageDescriptorInfo};
    computeDescriptorSetInfo.layout = m_computeShader->descriptor_set_layout(0);

    m_computeDescriptorSet = m_renderContext->create_descriptor_set(computeDescriptorSetInfo);

    eagle::CommandBufferCreateInfo commandBufferCreateInfo = {};
    commandBufferCreateInfo.level = eagle::CommandBufferLevel::MASTER;

    commandBufferCreateInfo.queue = m_renderContext->command_queue(eagle::CommandQueueType::GRAPHICS);
    m_graphicsCommandBuffer = m_renderContext->create_command_buffer(commandBufferCreateInfo);

    commandBufferCreateInfo.queue = m_renderContext->command_queue(eagle::CommandQueueType::COMPUTE);
    m_computeCommandBuffer = m_renderContext->create_command_buffer(commandBufferCreateInfo);


    //release image from graphics to compute
    {
        m_graphicsCommandBuffer->begin();

        eagle::ImageMemoryBarrier imageMemoryBarrier = {};
        imageMemoryBarrier.image = m_computeTexture->image();
        imageMemoryBarrier.allFrames = true;
        imageMemoryBarrier.srcQueue = m_renderContext->command_queue(eagle::CommandQueueType::GRAPHICS);
        imageMemoryBarrier.dstQueue = m_renderContext->command_queue(eagle::CommandQueueType::COMPUTE);

        imageMemoryBarrier.srcAccessMask = eagle::AccessFlagBits::SHADER_READ_BIT;
        imageMemoryBarrier.dstAccessMask = 0;

        imageMemoryBarrier.oldLayout = eagle::ImageLayout::UNDEFINED;
        imageMemoryBarrier.newLayout = eagle::ImageLayout::GENERAL;

        eagle::ImageMemoryBarrier imageMemoryBarriers[] = {imageMemoryBarrier};

        m_graphicsCommandBuffer->pipeline_barrier(
                imageMemoryBarriers,
                eagle::PipelineStageFlagsBits::FRAGMENT_SHADER_BIT,
                eagle::PipelineStageFlagsBits::BOTTOM_OF_PIPE_BIT
        );

        m_graphicsCommandBuffer->end();

        eagle::CommandBufferSubmitInfo submitInfo = {};
        eagle::CommandBuffer* commandBuffers[] = {m_graphicsCommandBuffer.get()};
        submitInfo.commandBuffers = commandBuffers;


        m_graphicsCommandBuffer->command_queue()->submit(submitInfo, nullptr);
        m_graphicsCommandBuffer->command_queue()->idle();
    }

    m_framesInFlight = m_renderContext->create_fence();
    m_renderFinished = m_renderContext->create_semaphore();
    m_frameAvailable = m_renderContext->create_semaphore();
    m_computeFinished = m_renderContext->create_semaphore();

    m_timer.start();
}

void ComputeApplication::step() {

    m_timer.update();

    m_framesInFlight->wait();

    if (!m_renderContext->prepare_frame(m_frameAvailable.get())){
        EG_WARNING("compute", "Failed to prepare frame, skipping");
        return;
    }

    submit_compute();
    submit_graphics();

    eagle::Semaphore* waitSemaphores[] = {m_renderFinished.get()};
    m_renderContext->present_frame(waitSemaphores);
}

ComputeApplication::~ComputeApplication() {
    m_listener.destroy();
}

void ComputeApplication::submit_compute() {

    m_computeCommandBuffer->begin();

    //acquire ownership
    {
        eagle::ImageMemoryBarrier imageOwnershipAcquireBarrier = {};
        imageOwnershipAcquireBarrier.image = m_computeTexture->image();

        imageOwnershipAcquireBarrier.srcQueue = m_renderContext->command_queue(eagle::CommandQueueType::GRAPHICS);
        imageOwnershipAcquireBarrier.dstQueue = m_renderContext->command_queue(eagle::CommandQueueType::COMPUTE);

        imageOwnershipAcquireBarrier.srcAccessMask = 0;
        imageOwnershipAcquireBarrier.dstAccessMask = eagle::AccessFlagBits::SHADER_WRITE_BIT;

        imageOwnershipAcquireBarrier.oldLayout = eagle::ImageLayout::UNDEFINED;
        imageOwnershipAcquireBarrier.newLayout = eagle::ImageLayout::GENERAL;

        eagle::ImageMemoryBarrier imageOwnershipAcquireBarriers[] = {imageOwnershipAcquireBarrier};

        m_computeCommandBuffer->pipeline_barrier(
                imageOwnershipAcquireBarriers,
                eagle::PipelineStageFlagsBits::TOP_OF_PIPE_BIT,
                eagle::PipelineStageFlagsBits::COMPUTE_SHADER_BIT
        );
    }


    m_computeCommandBuffer->bind_shader(m_computeShader.get());
    m_computeCommandBuffer->bind_descriptor_sets(m_computeDescriptorSet.get(), 0);
    m_computeCommandBuffer->dispatch(m_computeTexture->image()->width(), m_computeTexture->image()->height(), 1);

    {
        eagle::ImageMemoryBarrier imageMemoryBarrier = {};
        imageMemoryBarrier.image = m_computeTexture->image();

        imageMemoryBarrier.srcQueue = m_renderContext->command_queue(eagle::CommandQueueType::COMPUTE);
        imageMemoryBarrier.dstQueue = m_renderContext->command_queue(eagle::CommandQueueType::GRAPHICS);

        imageMemoryBarrier.srcAccessMask = eagle::AccessFlagBits::SHADER_WRITE_BIT;
        imageMemoryBarrier.dstAccessMask = 0;

        imageMemoryBarrier.oldLayout = eagle::ImageLayout::GENERAL;
        imageMemoryBarrier.newLayout = eagle::ImageLayout::SHADER_READ_ONLY_OPTIMAL;

        eagle::ImageMemoryBarrier imageMemoryBarriers[] = {imageMemoryBarrier};

        m_computeCommandBuffer->pipeline_barrier(
                imageMemoryBarriers,
                eagle::PipelineStageFlagsBits::COMPUTE_SHADER_BIT,
                eagle::PipelineStageFlagsBits::BOTTOM_OF_PIPE_BIT
        );
    }

    m_computeCommandBuffer->end();

    eagle::CommandBufferSubmitInfo submitInfo = {};
    eagle::CommandBuffer* commandBuffers[] = {m_computeCommandBuffer.get()};
    submitInfo.commandBuffers = commandBuffers;

//    eagle::Semaphore* waitSemaphores[] = {m_renderFinished.get()};
//    eagle::PipelineStageFlags waitStages[] {eagle::PipelineStageFlagsBits::COMPUTE_SHADER_BIT};
//    submitInfo.waitSemaphores = waitSemaphores;
//    submitInfo.waitStages = waitStages;

    eagle::Semaphore* signalSemaphores[] = {m_computeFinished.get()};
    submitInfo.signalSemaphores = signalSemaphores;
    m_computeCommandBuffer->command_queue()->submit(submitInfo, nullptr);

}

void ComputeApplication::submit_graphics() {
    m_graphicsCommandBuffer->begin();

    {
        eagle::ImageMemoryBarrier imageMemoryBarrier = {};
        imageMemoryBarrier.image = m_computeTexture->image();

        imageMemoryBarrier.srcQueue = m_renderContext->command_queue(eagle::CommandQueueType::COMPUTE);
        imageMemoryBarrier.dstQueue = m_renderContext->command_queue(eagle::CommandQueueType::GRAPHICS);

        imageMemoryBarrier.srcAccessMask = 0;
        imageMemoryBarrier.dstAccessMask = eagle::AccessFlagBits::SHADER_READ_BIT;

        imageMemoryBarrier.oldLayout = eagle::ImageLayout::GENERAL;
        imageMemoryBarrier.newLayout = eagle::ImageLayout::SHADER_READ_ONLY_OPTIMAL;

        eagle::ImageMemoryBarrier imageMemoryBarriers[] = {imageMemoryBarrier};

        m_graphicsCommandBuffer->pipeline_barrier(
                imageMemoryBarriers,
                eagle::PipelineStageFlagsBits::BOTTOM_OF_PIPE_BIT,
                eagle::PipelineStageFlagsBits::FRAGMENT_SHADER_BIT
        );
    }

    m_graphicsCommandBuffer->begin_render_pass(m_renderContext->main_render_pass(), m_renderContext->main_framebuffer());

    m_graphicsCommandBuffer->bind_shader(m_graphicsShader.get());
    m_graphicsCommandBuffer->bind_descriptor_sets(m_graphicsDescriptorSet.get(), 0);
    m_graphicsCommandBuffer->draw(3);

    m_graphicsCommandBuffer->end_render_pass();

    {
        eagle::ImageMemoryBarrier imageMemoryBarrier = {};
        imageMemoryBarrier.image = m_computeTexture->image();
        imageMemoryBarrier.srcQueue = m_renderContext->command_queue(eagle::CommandQueueType::GRAPHICS);
        imageMemoryBarrier.dstQueue = m_renderContext->command_queue(eagle::CommandQueueType::COMPUTE);

        imageMemoryBarrier.srcAccessMask = eagle::AccessFlagBits::SHADER_READ_BIT;
        imageMemoryBarrier.dstAccessMask = 0;

        imageMemoryBarrier.oldLayout = eagle::ImageLayout::UNDEFINED;
        imageMemoryBarrier.newLayout = eagle::ImageLayout::GENERAL;

        eagle::ImageMemoryBarrier imageMemoryBarriers[] = {imageMemoryBarrier};

        m_graphicsCommandBuffer->pipeline_barrier(
                imageMemoryBarriers,
                eagle::PipelineStageFlagsBits::FRAGMENT_SHADER_BIT,
                eagle::PipelineStageFlagsBits::BOTTOM_OF_PIPE_BIT
        );

    }

    m_graphicsCommandBuffer->end();

    eagle::CommandBufferSubmitInfo submitInfo = {};

    //command buffers to execute
    eagle::CommandBuffer* commandBuffers[] = {m_graphicsCommandBuffer.get()};
    submitInfo.commandBuffers = commandBuffers;

    //semaphores to wait for signal before starting
    //wait for color output
    eagle::PipelineStageFlags waitStages[] = {eagle::PipelineStageFlagsBits::TOP_OF_PIPE_BIT, eagle::PipelineStageFlagsBits::COLOR_ATTACHMENT_OUTPUT_BIT};
    eagle::Semaphore* waitSemaphores[] = {m_computeFinished.get(), m_frameAvailable.get()};

    submitInfo.waitStages = waitStages;
    submitInfo.waitSemaphores = waitSemaphores;

    //semaphores to signal when finished
    eagle::Semaphore* signalSemaphores[] = {m_renderFinished.get()};
    submitInfo.signalSemaphores = signalSemaphores;

    m_graphicsCommandBuffer->command_queue()->submit(submitInfo, m_framesInFlight.get());
}

