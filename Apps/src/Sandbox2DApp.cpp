#include "Sandbox2DApp.hpp"
#include "Utils/Log.hpp"
#include "Core/EntryPoint.hpp"
#include "Sandbox2DLayer.hpp"

sd::Application *sd::createApp() { return new Sandbox2DApp; }

void Sandbox2DApp::init() { pushLayer(sd::createRef<Sandbox2DLayer>()); }

void Sandbox2DApp::destroy() {}
