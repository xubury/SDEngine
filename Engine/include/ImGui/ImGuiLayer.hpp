#ifndef SD_IMGUI_LAYER_HPP
#define SD_IMGUI_LAYER_HPP

#include "Core/Layer.hpp"

namespace sd {

class SD_API ImGuiLayer : public Layer {
   public:
    ImGuiLayer();

    void begin();

    void end();

    void onAttach() override;

    void onDetech() override;

    void onEventPoll(const SDL_Event &) override;
   private:
    void setDarkThemeColor();

};

}  // namespace sd

#endif /* SD_IMGUI_LAYER_HPP */
