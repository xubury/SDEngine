#include "Sandbox2DApp.hpp"
#include "Core/Log.hpp"
#include "Core/EntryPoint.hpp"
#include "Sandbox2DLayer.hpp"

Sandbox2DApp::Sandbox2DApp() { pushLayer(sd::createRef<Sandbox2DLayer>()); }

sd::Application *sd::createApp() { return new Sandbox2DApp; }
