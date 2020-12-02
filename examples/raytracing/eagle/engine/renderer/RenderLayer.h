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
    virtual void handle_attach() override;

    virtual void handle_deattach() override;

    virtual void handle_update() override;

    virtual void handle_event(Event &e) override;

private:
    RenderMaster m_renderMaster;
    LayerEventDispatcher m_dispatcher;
};


EG_ENGINE_END

#endif //EAGLE_RENDERLAYER_H
