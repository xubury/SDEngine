#include "SDEditor.hpp"

IMPLEMENT_APP(SD::SDEditor);

namespace SD {

SDEditor::SDEditor() : Application("SD Editor", Device::API::OpenGL) {}

void SDEditor::OnInit()
{
    Application::OnInit();
    auto &settings = GetSettings();
    int viewport_width = settings.GetInteger("editor", "viewport width", 800);
    int viewport_height = settings.GetInteger("editor", "viewport height", 600);

    CreateImGuiLayer();
    m_graphics_layer->SetDebug(true);
    PushLayer(CreateLayer<EditorLayer>(&m_input, &m_scenes, &m_resources,
                                       m_graphics_layer, viewport_width,
                                       viewport_height));
}

void SDEditor::OnDestroy() { Application::OnDestroy(); }

}  // namespace SD
