#ifndef SD_IMGUI_LAYER_HPP
#define SD_IMGUI_LAYER_HPP

#include "Core/Layer.hpp"

union SDL_Event;

namespace SD {

class SD_API ImGuiLayer : public Layer {
   public:
    ImGuiLayer();
    ~ImGuiLayer();

    void Begin();

    void End();

    void OnInit() override;

    void OnPush() override;

    void OnPop() override;

    void OnEventProcess(const Event &event) override;
   private:
    void SetDarkThemeColor();
};

}  // namespace SD

#endif /* SD_IMGUI_LAYER_HPP */
