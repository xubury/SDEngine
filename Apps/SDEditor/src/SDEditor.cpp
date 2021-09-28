#include "SDEditor.hpp"

#include "Core/EntryPoint.hpp"
#include "EditorLayer.hpp"
#include "Utils/Log.hpp"

sd::Application *sd::createApp() { return new SDEditor; }

void SDEditor::init() { pushLayer(new EditorLayer(800, 600)); }

void SDEditor::destroy() {}
