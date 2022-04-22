#ifndef SD_INPUT_LAYER
#define SD_INPUT_LAYER

#include "Core/Layer.hpp"
#include "Core/InputDevice.hpp"

namespace SD {

class SD_CORE_API InputLayer : public Layer {
   public:
    InputLayer(InputDevice *input);

    void OnTick(float dt) override;

    void On(const KeyEvent &) override;
    void On(const MouseMotionEvent &) override;
    void On(const MouseButtonEvent &) override;
    void On(const MouseWheelEvent &) override;
   private:
    InputDevice *m_input;
};

}  // namespace SD

#endif  // !SD_INPUT_LAYER
