//
// Created by Novak on 22/03/2020.
//

#ifndef EAGLE_RENDERMASTER_H
#define EAGLE_RENDERMASTER_H

#include <eagle/engine/EngineGlobalDefinitions.h>
#include <eagle/engine/EventMaster.h>

EG_ENGINE_BEGIN

class RenderMaster {
public:
    typedef ImmediateEvent<> Event;
    typedef ImmediateEvent<Reference<CommandBuffer>&> CommandBufferEvent;

public:

    void init();
    void update();
    void deinit();

    void handle_window_resized(uint32_t width, uint32_t height);

    inline static RenderingContext& context() { return *s_context; }
public:
    static Event handle_context_init;
    static Event handle_context_deinit;
    static Event handle_frame_begin;
    static Event handle_frame_end;
    static Event handle_context_recreated;
    static CommandBufferEvent handle_command_buffer_begin;
    static CommandBufferEvent handle_command_buffer_end;
    static CommandBufferEvent handle_command_buffer_main_render_pass;

private:
    static Reference<RenderingContext> s_context;
};

EG_ENGINE_END

#endif //EAGLE_RENDERMASTER_H
