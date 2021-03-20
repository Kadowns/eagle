//
//  Created by Novak 05/06/2019
//  Code HEAVILY based on Hazel layers: https://github.com/TheCherno/Hazel
//

#ifndef EAGLE_LAYER_H
#define EAGLE_LAYER_H

#include "core_global_definitions.h"
#include "eagle/events/event.h"

namespace eagle {

class Layer {

public:
    Layer()                         = default;
    virtual ~Layer()                = default;

    virtual void handle_attach() {}
    virtual void handle_detach() {}
    virtual void handle_update() {}
};

}

#endif //EAGLE_LAYER_H
