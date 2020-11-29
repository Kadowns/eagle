//
// Created by Novak on 10/11/2019.
//

#ifndef EAGLE_INPUTLAYER_H
#define EAGLE_INPUTLAYER_H

#include "CoreGlobalDefinitions.h"
#include "Layer.h"

EG_BEGIN

class InputLayer : public Layer {
public:

    InputLayer() = default;

    virtual ~InputLayer() = default;

    virtual void handle_attach() override;

    virtual void handle_deattach() override;

    virtual void handle_update() override;

    virtual void handle_event(Event &e) override;

private:

    LayerEventDispatcher m_dispatcher;
};

EG_END

#endif //EAGLE_INPUTLAYER_H
