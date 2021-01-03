//
// Created by Novak on 22/03/2020.
//

#include <eagle/engine/renderer/RenderMaster.h>

EG_ENGINE_BEGIN

Reference<RenderingContext> RenderMaster::s_context;

void RenderMaster::init() {
    Window& window = Application::instance().window();
    m_eventBus = &Application::instance().event_bus();

    s_context = std::make_shared<VulkanContext>();
    s_context->init(&window, m_eventBus);

    m_eventBus->emit(OnRenderContextInit{s_context.get()});
}

void RenderMaster::update() {
    if (!s_context->prepare_frame()){
        EG_WARNING("Failed to prepare frame!");
        return;
    }

    m_eventBus->emit(OnRenderFrameBegin{s_context.get()});

    auto commandBuffer = s_context->main_command_buffer();
    commandBuffer->begin();

    m_eventBus->emit(OnRenderCommandBufferBegin{s_context.get(), commandBuffer.get()});

    commandBuffer->begin_render_pass(s_context->main_render_pass(), s_context->main_frambuffer());

    m_eventBus->emit(OnRenderCommandBufferMainRenderPass{s_context.get(), commandBuffer.get()});

    commandBuffer->end_render_pass();

    m_eventBus->emit(OnRenderCommandBufferEnd{s_context.get(), commandBuffer.get()});

    commandBuffer->finish();

    s_context->submit_command_buffer(commandBuffer);

    m_eventBus->emit(OnRenderFrameEnd{s_context.get()});

    s_context->present_frame();
}

void RenderMaster::deinit() {
    m_eventBus->emit(OnRenderContextDeinit{s_context.get()});
    s_context->deinit();
}

EG_ENGINE_END
