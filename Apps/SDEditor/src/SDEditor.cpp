#include "SDEditor.hpp"

IMPLEMENT_APP(SD::SDEditor);

namespace SD {

SDEditor::SDEditor() : Application("SD Editor", GraphicsAPI::OpenGL) {}

void SDEditor::OnStart() {
    int viewport_width = ini->GetInteger("editor", "viewport width", 800);
    int viewport_height = ini->GetInteger("editor", "viewport height", 600);

#ifdef SD_IMGUI_IMPORT
    // for DLL context
    ImGui::SetCurrentContext(GetImGuiLayer()->GetContext());
#endif

    m_layer = CreateLayer<EditorLayer>(viewport_width, viewport_height);
    PushLayer(m_layer);
}

void SDEditor::OnExit() {
    ini->SetInteger("editor", "viewport width", m_layer->GetViewportWidth());
    ini->SetInteger("editor", "viewport height", m_layer->GetViewportHeight());

    DestroyLayer(m_layer);
}

}  // namespace SD
