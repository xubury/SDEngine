#ifndef SD_INPUT_LAYER
#define SD_INPUT_LAYER

#include "Core/Layer.hpp"

namespace SD {

class SD_CORE_API InputLayer : public Layer {
   public:
    InputLayer();

    void OnTick(float dt) override;

    void On(const KeyEvent &) override;
    void On(const MouseMotionEvent &) override;
    void On(const MouseButtonEvent &) override;
    void On(const MouseWheelEvent &) override;
};

}  // namespace SD

#endif  // !SD_INPUT_LAYER
