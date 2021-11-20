#include "SDEditor.hpp"

#include "Core/EntryPoint.hpp"

IMPLEMENT_APP(SDEditor);

namespace SD {

void SDEditor::OnStart() {
    int viewport_width = ini->GetInteger("editor", "viewport width", 800);
    int viewport_height = ini->GetInteger("editor", "viewport height", 600);
    m_layer = new EditorLayer(viewport_width, viewport_height, window->GetMSAA());
    PushLayer(m_layer);
}

void SDEditor::OnExit() {
    ini->SetInteger("editor", "viewport width", m_layer->GetViewportWidth());
    ini->SetInteger("editor", "viewport height", m_layer->GetViewportHeight());

    DestroyLayer(m_layer);
}

}  // namespace SD
