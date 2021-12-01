#ifndef SD_IMGUI_LAYER_HPP
#define SD_IMGUI_LAYER_HPP

#include "Core/Export.hpp"
#include "Core/Layer.hpp"
#include "ImGui/ImGuiWidget.hpp"

namespace SD {

class SD_CORE_API ImGuiLayer : public Layer {
   public:
    ImGuiLayer();

    ~ImGuiLayer();

    void Begin();

    void End();

    void OnInit() override;

    void OnPush() override;

    void OnPop() override;

    void OnEventProcess(const Event &event) override;

    ImGuiContext *GetContext() { return m_context; }

   private:
    void SetDarkThemeColor();
    ImGuiContext *m_context;
};

}  // namespace SD

#endif /* SD_IMGUI_LAYER_HPP */
