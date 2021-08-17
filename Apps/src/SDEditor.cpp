#include "SDEditor.hpp"
#include "Common/Log.hpp"
#include "Core/EntryPoint.hpp"
#include "Sandbox2DLayer.hpp"
#include "EditorLayer.hpp"

sd::Application *sd::createApp() { return new SDEditor; }

void SDEditor::init() {
    // pushLayer(new Sandbox2DLayer());

    pushLayer(new EditorLayer());
}

void SDEditor::destroy() {}
