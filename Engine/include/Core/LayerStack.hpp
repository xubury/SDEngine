#ifndef SD_LAYER_STACK_HPP
#define SD_LAYER_STACK_HPP

#include "Utility/Base.hpp"
#include "Core/Layer.hpp"
#include <vector>

namespace sd {

class SD_API LayerStack {
   public:
    LayerStack();

    ~LayerStack();

    Layer *pushLayer(Layer *layer);

    Layer *pushOverlay(Layer *layer);

    void popLayer(Layer *layer);

    void popOverlay(Layer *layer);

    bool hasLayer(Layer *layer) const;

    std::vector<Layer *>::iterator begin() { return m_layers.begin(); }

    std::vector<Layer *>::iterator end() { return m_layers.end(); }

    std::vector<Layer *>::reverse_iterator rbegin() {
        return m_layers.rbegin();
    }

    std::vector<Layer *>::reverse_iterator rend() { return m_layers.rend(); }

    std::vector<Layer *>::const_iterator begin() const {
        return m_layers.begin();
    }

    std::vector<Layer *>::const_iterator end() const { return m_layers.end(); }

    std::vector<Layer *>::const_reverse_iterator rbegin() const {
        return m_layers.rbegin();
    }

    std::vector<Layer *>::const_reverse_iterator rend() const {
        return m_layers.rend();
    }

   private:
    std::vector<Layer *> m_layers;
    size_t m_layerInsertId;
};

}  // namespace sd

#endif /* SD_LAYER_STACK_HPP */
