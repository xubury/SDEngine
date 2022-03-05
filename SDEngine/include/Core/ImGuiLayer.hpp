#ifndef SD_IMGUI_LAYER_HPP
#define SD_IMGUI_LAYER_HPP

#include "Core/Export.hpp"
#include "Core/Layer.hpp"
#include "Core/Window.hpp"
#include "ImGui/ImGuiWidget.hpp"

namespace SD {

class SD_CORE_API ImGuiLayer : public Layer {
   public:
    ImGuiLayer(Window *window);

    ~ImGuiLayer();

    void Begin();

    void End();

    void OnInit() override;

    void OnPush() override;

    void OnPop() override;

    ImGuiContext *GetContext() const { return m_context; }

   private:
    void SetDarkThemeColor();
    ImGuiContext *m_context;
    Window *m_window;
    std::vector<HandlerRegistration> m_handlers;
};

}  // namespace SD

#endif /* SD_IMGUI_LAYER_HPP */
