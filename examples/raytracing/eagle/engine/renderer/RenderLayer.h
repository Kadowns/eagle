//
// Created by Novak on 17/03/2020.
//

#ifndef EAGLE_RENDERLAYER_H
#define EAGLE_RENDERLAYER_H

#include <eagle/engine/EngineGlobalDefinitions.h>
#include <eagle/engine/renderer/RenderMaster.h>

EG_ENGINE_BEGIN

class RenderLayer : public Layer {

public:
    virtual void handle_attach(EventBus<EventStream>* eventBus) override;

    virtual void handle_detach() override;

    virtual void handle_update() override;

private:
    RenderMaster m_renderMaster;
    EventListener<RenderLayer, EventBus<EventStream>> m_listener;
};


EG_ENGINE_END

#endif //EAGLE_RENDERLAYER_H
