#ifndef SD_IMGUI_LAYER_HPP
#define SD_IMGUI_LAYER_HPP

#include "Core/Layer.hpp"
#include "Graphics/Texture.hpp"
#include "Renderer/RenderTarget.hpp"

namespace sd {

class ImGuiLayer : public Layer {
   public:
    ImGuiLayer();

    void begin();

    void end();

    void onAttach() override;

    void onDetech() override;

    void onImGui() override;

   private:
    void setDarkThemeColor();

    Ref<Texture> m_texture;
    Ref<RenderTarget> m_target;
    int m_width;
    int m_height;
};

}  // namespace sd

#endif /* SD_IMGUI_LAYER_HPP */
