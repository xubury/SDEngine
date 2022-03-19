#include "SDEditor.hpp"
#include "sol/sol.hpp"

#include "Asset/AssetStorage.hpp"

IMPLEMENT_APP(SD::SDEditor);

namespace SD {

SDEditor::SDEditor() : Application("SD Editor", Device::API::OpenGL) {}

void SDEditor::OnInit()
{
    Application::OnInit();

    // lua test
    sol::state lua;
    int x = 0;
    lua.set_function("beep", [&x] { ++x; });
    lua.script("beep()");
    SD_CORE_INFO("lua test: x={}", x);

    auto &settings = GetSettings();
    int viewport_width = settings.GetInteger("editor", "viewport width", 800);
    int viewport_height = settings.GetInteger("editor", "viewport height", 600);
#ifdef SD_IMGUI_IMPORT
    // for DLL context
    ImGui::SetCurrentContext(GetImGuiLayer()->GetContext());
#endif

    m_graphics_layer = CreateLayer<GraphicsLayer>(
        viewport_width, viewport_height, m_window->GetMSAA());
    m_graphics_layer->SetDebug(true);
    PushLayer(m_graphics_layer);

    m_editor_layer = CreateLayer<EditorLayer>(m_graphics_layer, viewport_width,
                                              viewport_height);

    PushLayer(m_editor_layer);
}

void SDEditor::OnDestroy() { Application::OnDestroy(); }

}  // namespace SD
