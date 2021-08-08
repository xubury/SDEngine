#include "Sandbox2DApp.hpp"
#include "Utils/Log.hpp"
#include "Core/EntryPoint.hpp"
#include "Sandbox2DLayer.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Renderer/Renderer3D.hpp"

sd::Application *sd::createApp() { return new Sandbox2DApp; }

void Sandbox2DApp::init() {
    sd::Graphics::init();
    sd::Renderer::init(sd::API::OpenGL);

    sd::Renderer2D::init();
    sd::Renderer3D::init();

    pushLayer(sd::createRef<Sandbox2DLayer>());
}

void Sandbox2DApp::destroy() {}
