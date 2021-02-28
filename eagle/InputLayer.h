//
// Created by Novak on 10/11/2019.
//

#ifndef EAGLE_INPUTLAYER_H
#define EAGLE_INPUTLAYER_H

#include "CoreGlobalDefinitions.h"
#include "Layer.h"

namespace eagle {

class InputLayer : public Layer {
public:

    InputLayer() = default;

    virtual ~InputLayer() = default;

    virtual void handle_attach() override;

    virtual void handle_detach() override;

    virtual void handle_update() override;

};

}

#endif //EAGLE_INPUTLAYER_H
