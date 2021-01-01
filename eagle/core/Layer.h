//
//  Created by Novak 05/06/2019
//  Code HEAVILY based on Hazel layers: https://github.com/TheCherno/Hazel
//

#ifndef EAGLE_LAYER_H
#define EAGLE_LAYER_H

#include "CoreGlobalDefinitions.h"
#include "events/EventBus.h"

EG_BEGIN

class Layer {

public:
    Layer()                         = default;
    virtual ~Layer()                = default;

    virtual void handle_attach(Eagle::EventBus* eventBus) {}
    virtual void handle_detach()      {}
    virtual void handle_update()        {}
};

EG_END

#endif //EAGLE_LAYER_H
