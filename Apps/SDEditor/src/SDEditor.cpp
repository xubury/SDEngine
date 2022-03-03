#include "SDEditor.hpp"
#include "sol/sol.hpp"
#include "Renderer/Renderer.hpp"

#include "Asset/AssetStorage.hpp"

IMPLEMENT_APP(SD::SDEditor);

namespace SD {

SDEditor::SDEditor() : Application("SD Editor", GraphicsAPI::OpenGL) {}

void SDEditor::OnInit() {
    Application::OnInit();

    // lua test
    sol::state lua;
    int x = 0;
    lua.set_function("beep", [&x] { ++x; });
    lua.script("beep()");
    SD_CORE_INFO("lua test: x={}", x);

    int viewport_width = setting->GetInteger("editor", "viewport width", 800);
    int viewport_height = setting->GetInteger("editor", "viewport height", 600);
#ifdef SD_IMGUI_IMPORT
    // for DLL context
    ImGui::SetCurrentContext(GetImGuiLayer()->GetContext());
#endif

    // Renderer::Get().SetSize(viewport_width, viewport_height);
    m_layer = CreateLayer<EditorLayer>(viewport_width, viewport_height,
                                       Window::Get().GetMSAA());
    PushLayer(m_layer);
}

void SDEditor::OnDestroy() { Application::OnDestroy(); }

}  // namespace SD
