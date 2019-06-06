//
// Created by Novak on 05/06/2019.
//

#ifndef EAGLE_LAYERSTACK_H
#define EAGLE_LAYERSTACK_H

#include <memory>
#include <vector>

#include "Core.h"
#include "Layer.h"

_EAGLE_BEGIN

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



_EAGLE_END

#endif //EAGLE_LAYERSTACK_H
