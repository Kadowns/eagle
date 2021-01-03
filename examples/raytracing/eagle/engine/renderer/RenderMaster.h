//
// Created by Novak on 22/03/2020.
//

#ifndef EAGLE_RENDERMASTER_H
#define EAGLE_RENDERMASTER_H

#include <eagle/engine/EngineGlobalDefinitions.h>

EG_ENGINE_BEGIN

struct OnRenderingEvent {
    RenderingContext* context;
};

struct OnCommandBufferEvent {
    CommandBuffer* commandBuffer;
};

struct OnRenderContextInit : public OnRenderingEvent {};
struct OnRenderContextDeinit : public OnRenderingEvent {};
struct OnRenderFrameBegin : public OnRenderingEvent {};
struct OnRenderFrameEnd : public OnRenderingEvent {};
struct OnRenderCommandBufferBegin : public OnRenderingEvent, public OnCommandBufferEvent {};
struct OnRenderCommandBufferEnd : public OnRenderingEvent, public OnCommandBufferEvent {};
struct OnRenderCommandBufferMainRenderPass : public OnRenderingEvent, public OnCommandBufferEvent {};

class RenderMaster {
public:

    void init();
    void update();
    void deinit();

    inline static RenderingContext& context() { return *s_context; }
private:
    static Reference<RenderingContext> s_context;
    EventBus* m_eventBus;
};

EG_ENGINE_END

#endif //EAGLE_RENDERMASTER_H
