#include "Sandbox2DApp.hpp"
#include "Core/Log.hpp"
#include "Core/EntryPoint.hpp"
#include "Sandbox2DLayer.hpp"
#include "Graphics/Renderer2D.hpp"

sd::Application *sd::createApp() { return new Sandbox2DApp; }

void Sandbox2DApp::init() {
    sd::Renderer2D::init();
    pushLayer(sd::createRef<Sandbox2DLayer>());
}

void Sandbox2DApp::destroy() { sd::Renderer2D::destory(); }
