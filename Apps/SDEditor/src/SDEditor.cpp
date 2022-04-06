#include "SDEditor.hpp"
#include "Script/ScriptLayer.hpp"

#include "Asset/AssetStorage.hpp"

IMPLEMENT_APP(SD::SDEditor);

namespace SD {

SDEditor::SDEditor() : Application("SD Editor", Device::API::OpenGL) {}

void SDEditor::OnInit()
{
    Application::OnInit();
    auto &settings = GetSettings();
    int viewport_width = settings.GetInteger("editor", "viewport width", 800);
    int viewport_height = settings.GetInteger("editor", "viewport height", 600);
#ifdef SD_IMGUI_IMPORT
    // for DLL context
    ImGui::SetCurrentContext(GetImGuiLayer()->GetContext());
#endif

    m_graphics_layer->SetDebug(true);
    PushLayer(CreateLayer<ScriptLayer>());
    m_editor_layer = CreateLayer<EditorLayer>(m_graphics_layer, viewport_width,
                                              viewport_height);

    PushLayer(m_editor_layer);
}

void SDEditor::OnDestroy() { Application::OnDestroy(); }

}  // namespace SD
