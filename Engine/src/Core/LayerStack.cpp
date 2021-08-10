#include "Core/LayerStack.hpp"
#include <algorithm>

namespace sd {

LayerStack::LayerStack() : m_layerInsertId(0) {}

LayerStack::~LayerStack() {
    for (Layer *layer : m_layers) {
        layer->onDetech();
        delete layer;
    }
    m_layers.clear();
}

void LayerStack::pushLayer(Layer *layer) {
    m_layers.emplace(m_layers.begin() + m_layerInsertId, layer);
    ++m_layerInsertId;
    layer->onAttach();
}

void LayerStack::pushOverlay(Layer *overlay) {
    m_layers.emplace_back(overlay);
    overlay->onAttach();
}

void LayerStack::popLayer(Layer *layer) {
    auto iter =
        std::find(m_layers.begin(), m_layers.begin() + m_layerInsertId, layer);
    if (iter != m_layers.begin() + m_layerInsertId) {
        layer->onDetech();
        m_layers.erase(iter);
        --m_layerInsertId;
    }
}

void LayerStack::popOverlay(Layer *overlay) {
    auto iter =
        std::find(m_layers.begin() + m_layerInsertId, m_layers.end(), overlay);
    if (iter != m_layers.end()) {
        overlay->onDetech();
        m_layers.erase(iter);
    }
}

bool LayerStack::hasLayer(const Layer *layer) const {
    auto iter = std::find(m_layers.begin(), m_layers.end(), layer);
    return iter != m_layers.end();
}

}  // namespace sd
