//
// Created by Novak on 05/06/2019.
//

#include <algorithm>

#include "eagle/core/LayerStack.h"
#include <eagle/core/Log.h>

_EAGLE_BEGIN

void LayerStack::emplace_back(std::shared_ptr<Layer> layer) {
    EG_CORE_TRACE("Emplacing back a new layer!");
    m_layers.emplace_back(layer);
    layer->handle_attach();
}

void LayerStack::pop_layer(std::shared_ptr<Layer> layer) {
    EG_CORE_TRACE("Popping a layer!");
    auto it = std::find(m_layers.begin(), m_layers.end(), layer);
    if (it != m_layers.end()){
        m_layers.erase(it);
        layer->handle_deattach();
        EG_CORE_TRACE("Layer popped!");
    } else {
        EG_CORE_TRACE("Layer not found!");
    }
}

LayerStack::LayerStack(const std::vector<std::shared_ptr<Layer>>& layers) {
    for (auto& it :layers){
        emplace_back(it);
    }
}

void LayerStack::emplace_front(std::shared_ptr<Layer> layer) {
    EG_CORE_TRACE("Emplacing front a new layer!");
    m_layers.emplace(m_layers.begin(), layer);
    layer->handle_attach();
}

LayerStack::~LayerStack() = default;


_EAGLE_END

