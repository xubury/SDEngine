#include "Core/LayerStack.hpp"
#include <algorithm>

namespace SD {

LayerStack::LayerStack() : m_layerInsertId(0) {}

LayerStack::~LayerStack() { m_layers.clear(); }

void LayerStack::pushLayer(Layer *layer) {
    m_layers.emplace(m_layers.begin() + m_layerInsertId, layer);
    ++m_layerInsertId;
    layer->onPush();
}

void LayerStack::pushOverlay(Layer *overlay) {
    m_layers.emplace_back(overlay);
    overlay->onPush();
}

void LayerStack::popLayer(Layer *layer) {
    auto iter = m_layers.begin();
    for (; iter != m_layers.end(); iter++) {
        if (*iter == layer) {
            break;
        }
    }
    if (iter != m_layers.end()) {
        layer->onPop();
        m_layers.erase(iter);
        size_t id = std::distance(iter, m_layers.begin());
        if (id < m_layerInsertId) {
            --m_layerInsertId;
        }
    } else {
        SD_CORE_ERROR("LayerStack::popLayer Failed! No layer found!");
    }
}

bool LayerStack::hasLayer(Layer *layer) const {
    auto iter = std::find(m_layers.begin(), m_layers.end(), layer);
    return iter != m_layers.end();
}

}  // namespace SD
