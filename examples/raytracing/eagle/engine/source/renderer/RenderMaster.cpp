//
// Created by Novak on 22/03/2020.
//

#include <eagle/engine/renderer/RenderMaster.h>



EG_ENGINE_BEGIN

RenderMaster::Event RenderMaster::handle_context_init;
RenderMaster::Event RenderMaster::handle_context_deinit;
RenderMaster::Event RenderMaster::handle_frame_begin;
RenderMaster::Event RenderMaster::handle_frame_end;
RenderMaster::Event RenderMaster::handle_context_recreated;
RenderMaster::CommandBufferEvent RenderMaster::handle_command_buffer_begin;
RenderMaster::CommandBufferEvent RenderMaster::handle_command_buffer_end;
RenderMaster::CommandBufferEvent RenderMaster::handle_command_buffer_main_render_pass;

Reference<RenderingContext> RenderMaster::s_context;

void RenderMaster::init() {
    Window& window = Application::instance().window();

    s_context = std::make_shared<VulkanContext>();
    s_context->init(&window);
    s_context->set_recreation_callback([&](){
        handle_context_recreated();
    });

    handle_context_init();
}

void RenderMaster::update() {
    if (!s_context->prepare_frame()){
        EG_WARNING("Failed to prepare frame!");
        return;
    }

    handle_frame_begin();

    auto commandBuffer = s_context->create_command_buffer();
    commandBuffer->begin();

    handle_command_buffer_begin(commandBuffer);

    commandBuffer->begin_render_pass(s_context->main_render_pass(), s_context->main_frambuffer());

    handle_command_buffer_main_render_pass(commandBuffer);

    commandBuffer->end_render_pass();

    handle_command_buffer_end(commandBuffer);

    commandBuffer->finish();
    commandBuffer->submit();

    handle_frame_end();

    s_context->present_frame();
}

void RenderMaster::deinit() {
    handle_context_deinit();
    s_context->deinit();
}

void RenderMaster::handle_window_resized(uint32_t width, uint32_t height) {
    s_context->handle_window_resized(width, height);
}

EG_ENGINE_END
