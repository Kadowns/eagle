//
// Created by Novak on 05/06/2019.
//

#include <algorithm>

#include "LayerStack.h"
#include "Log.h"

namespace eagle {

void LayerStack::emplace_back(std::shared_ptr<Layer> layer) {
    EG_TRACE("eagle", "Emplacing back a new layer!");
    m_layers.emplace_back(layer);

    if (!m_initialized)
        return;

    layer->handle_attach();
}

void LayerStack::pop_layer(std::shared_ptr<Layer> layer) {
    EG_TRACE("eagle", "Popping a layer!");
    auto it = std::find(m_layers.begin(), m_layers.end(), layer);
    if (it != m_layers.end()){
        m_layers.erase(it);

        if (!m_initialized)
            return;

        layer->handle_detach();
        EG_TRACE("eagle", "Layer popped!");
    } else {
        EG_TRACE("eagle", "Layer not found!");
    }
}

LayerStack::LayerStack(const std::vector<std::shared_ptr<Layer>>& layers) {
    emplace(layers);
}

void LayerStack::emplace_front(std::shared_ptr<Layer> layer) {
    EG_TRACE("eagle", "Emplacing front a new layer!");
    m_layers.emplace(m_layers.begin(), layer);

    if (!m_initialized)
        return;

    layer->handle_attach();
}

void LayerStack::emplace(const std::vector<std::shared_ptr<Layer>>& layers) {
    for (auto& it :layers){
        emplace_back(it);
    }
}

void LayerStack::init() {

    if (m_initialized) return;

    EG_TRACE("eagle", "Initializing layer stack!");

    m_initialized = true;
    for (auto& layer : m_layers){
        layer->handle_attach();
    }
}

void LayerStack::deinit() {
    if (!m_initialized) return;

    EG_TRACE("eagle", "Deinitializing layer stack!");

    for (auto& layer : m_layers){
        layer->handle_detach();
    }
    m_layers.clear();

    m_initialized = false;
}


}

