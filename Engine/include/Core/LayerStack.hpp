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

    Ref<Layer> pushLayer(const Ref<Layer> &layer);

    Ref<Layer> pushOverlay(const Ref<Layer> &layer);

    void popLayer(const Ref<Layer> &layer);

    void popOverlay(const Ref<Layer> &layer);

    bool hasLayer(const Ref<Layer> &layer) const;

    std::vector<Ref<Layer>>::iterator begin() { return m_layers.begin(); }

    std::vector<Ref<Layer>>::iterator end() { return m_layers.end(); }

    std::vector<Ref<Layer>>::reverse_iterator rbegin() {
        return m_layers.rbegin();
    }

    std::vector<Ref<Layer>>::reverse_iterator rend() { return m_layers.rend(); }

    std::vector<Ref<Layer>>::const_iterator begin() const {
        return m_layers.begin();
    }

    std::vector<Ref<Layer>>::const_iterator end() const {
        return m_layers.end();
    }

    std::vector<Ref<Layer>>::const_reverse_iterator rbegin() const {
        return m_layers.rbegin();
    }

    std::vector<Ref<Layer>>::const_reverse_iterator rend() const {
        return m_layers.rend();
    }

   private:
    std::vector<Ref<Layer>> m_layers;
    size_t m_layerInsertId;
};

}  // namespace sd

#endif /* SD_LAYER_STACK_HPP */
