#include "Sandbox2DApp.hpp"
#include "Common/Log.hpp"
#include "Core/EntryPoint.hpp"
#include "Sandbox2DLayer.hpp"
#include "EditorLayer.hpp"

sd::Application *sd::createApp() { return new Sandbox2DApp; }

void Sandbox2DApp::init() {
    // pushLayer(new Sandbox2DLayer());

    pushLayer(new EditorLayer());
}

void Sandbox2DApp::destroy() {}
