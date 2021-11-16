#ifndef SD_IMGUI_LAYER_HPP
#define SD_IMGUI_LAYER_HPP

#include "Core/Layer.hpp"

union SDL_Event;

namespace SD {

class SD_API ImGuiLayer : public Layer {
   public:
    ImGuiLayer();

    void begin();

    void end();

    void onPush() override;

    void onPop() override;

    void onEventProcess(const Event &event) override;
   private:
    void setDarkThemeColor();
};

}  // namespace SD

#endif /* SD_IMGUI_LAYER_HPP */
