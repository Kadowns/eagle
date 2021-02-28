//
// Created by Novak on 05/06/2019.
//

#ifndef EAGLE_LAYERSTACK_H
#define EAGLE_LAYERSTACK_H

#include "CoreGlobalDefinitions.h"
#include "Layer.h"
#include <eagle/events/EventBus.h>

namespace eagle {

class LayerStack {

public:

    explicit LayerStack(const std::vector<std::shared_ptr<Layer>>& initialLayers);
    LayerStack() = default;
    ~LayerStack() = default;


    void init();
    void deinit();
    void emplace_back(std::shared_ptr<Layer> layer);
    void emplace_front(std::shared_ptr<Layer> layer);
    void emplace(const std::vector<std::shared_ptr<Layer>>& layers);
    void pop_layer(std::shared_ptr<Layer> layer);

    std::vector<std::shared_ptr<Layer>>::iterator begin()   { return m_layers.begin();   }
    std::vector<std::shared_ptr<Layer>>::iterator end()     { return m_layers.end();     }

private:

    std::vector<std::shared_ptr<Layer>> m_layers;

    bool m_initialized = false;
};



}

#endif //EAGLE_LAYERSTACK_H
