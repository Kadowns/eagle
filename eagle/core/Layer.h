//
//  Created by Novak 05/06/2019
//  Code HEAVILY based on Hazel layers: https://github.com/TheCherno/Hazel
//

#ifndef EAGLE_LAYER_H
#define EAGLE_LAYER_H

#include "CoreGlobalDefinitions.h"
#include "events/Event.h"

EG_BEGIN

class Layer {

public:
    Layer()                         = default;
    virtual ~Layer()                = default;

    virtual void handle_attach()        {}
    virtual void handle_deattach()      {}
    virtual void handle_update()        {}
    virtual void handle_event(Event& e) {}
};

EG_END

#endif //EAGLE_LAYER_H
