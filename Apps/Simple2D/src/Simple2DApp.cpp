#include "Simple2DApp.hpp"
#include "Core/Log.hpp"
#include "Core/EntryPoint.hpp"

Simple2DApp::Simple2DApp() {}

sd::Application *sd::createApp() { return new Simple2DApp; }
