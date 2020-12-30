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

    virtual void handle_attach(EventBus<EventStream>* eventBus) override;

    virtual void handle_detach() override;

    virtual void handle_update() override;

};

EG_END

#endif //EAGLE_INPUTLAYER_H
