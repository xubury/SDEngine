#include "Core/LayerStack.hpp"
#include <algorithm>

namespace SD {

LayerStack::LayerStack() : m_layerInsertId(0) {}

LayerStack::~LayerStack() {
    for (auto &layer : m_layers) {
        layer->onDetech();
    }
    m_layers.clear();
}

Layer *LayerStack::pushLayer(Layer *layer) {
    m_layers.emplace(m_layers.begin() + m_layerInsertId, layer);
    ++m_layerInsertId;
    layer->onAttach();
    return layer;
}

Layer *LayerStack::pushOverlay(Layer *overlay) {
    m_layers.emplace_back(overlay);
    overlay->onAttach();
    return overlay;
}

void LayerStack::popLayer(Layer *layer) {
    auto iter =
        std::find(m_layers.begin(), m_layers.begin() + m_layerInsertId, layer);
    if (iter != m_layers.begin() + m_layerInsertId) {
        layer->onDetech();
        m_layers.erase(iter);
        --m_layerInsertId;
    } else {
        SD_CORE_ERROR("LayerStack::popLayer Failed! No layer found!");
    }
}

void LayerStack::popOverlay(Layer *overlay) {
    auto iter =
        std::find(m_layers.begin() + m_layerInsertId, m_layers.end(), overlay);
    if (iter != m_layers.end()) {
        overlay->onDetech();
        m_layers.erase(iter);
    } else {
        SD_CORE_ERROR("LayerStack::popOverLay Failed! No layer found!");
    }
}

bool LayerStack::hasLayer(Layer *layer) const {
    auto iter = std::find(m_layers.begin(), m_layers.end(), layer);
    return iter != m_layers.end();
}

}  // namespace SD
