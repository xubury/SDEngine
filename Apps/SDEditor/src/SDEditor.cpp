#include "SDEditor.hpp"
#include "sol/sol.hpp"

IMPLEMENT_APP(SD::SDEditor);

namespace SD {

SDEditor::SDEditor() : Application("SD Editor", GraphicsAPI::OpenGL) {}

void SDEditor::OnStart() {
    // lua test
    sol::state lua;
    int x = 0;
    lua.set_function("beep", [&x] { ++x; });
    lua.script("beep()");
    SD_CORE_INFO("lua test: x={}", x);

    int viewport_width = ini->GetInteger("editor", "viewport width", 800);
    int viewport_height = ini->GetInteger("editor", "viewport height", 600);

    renderer = CreateRef<Renderer>(
        Viewport(0, 0, viewport_width, viewport_height), device.get());
#ifdef SD_IMGUI_IMPORT
    // for DLL context
    ImGui::SetCurrentContext(GetImGuiLayer()->GetContext());
#endif

    m_layer = CreateLayer<EditorLayer>(viewport_width, viewport_height,
                                       GetWindow().GetMSAA());
    PushLayer(m_layer);
    // asset module test
    TextureAsset *texture_asset =
        asset->CreateAsset<TextureAsset>("test.asset");
    texture_asset->SetTexturePath(std::filesystem::current_path() /
                                  "assets/textures/awesomeface.png");

    asset->SaveAsset<TextureAsset>(texture_asset->GetId());
    asset->Unload<TextureAsset>(texture_asset->GetId());

    TextureAsset *loaded = asset->LoadAsset<TextureAsset>("test.asset");
    SD_CORE_TRACE("texture path:{} path:{}", loaded->GetTexturePath(),
                  loaded->GetPath());
}

void SDEditor::OnExit() {
    ini->SetInteger("editor", "viewport width", m_layer->GetViewportWidth());
    ini->SetInteger("editor", "viewport height", m_layer->GetViewportHeight());
}

}  // namespace SD
