//
// Created by Novak on 05/06/2019.
//

#ifndef EAGLE_LAYERSTACK_H
#define EAGLE_LAYERSTACK_H

#include <memory>
#include <vector>

#include "CoreGlobalDefinitions.h"
#include "Layer.h"

EG_BEGIN

class LayerStack {

public:

    explicit LayerStack(const std::vector<Reference<Layer>>& initialLayers);
    LayerStack() = default;
    ~LayerStack() = default;


    void init();
    void deinit();
    void emplace_back(Reference<Layer> layer);
    void emplace_front(Reference<Layer> layer);
    void emplace(const std::vector<Reference<Layer>>& layers);
    void pop_layer(Reference<Layer> layer);


    std::vector<Reference<Layer>>::iterator begin()   { return m_layers.begin();   }
    std::vector<Reference<Layer>>::iterator end()     { return m_layers.end();     }

private:

    std::vector<Reference<Layer>> m_layers;

    bool m_initialized = false;

};



EG_END

#endif //EAGLE_LAYERSTACK_H
