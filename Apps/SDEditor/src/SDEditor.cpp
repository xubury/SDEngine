#include "SDEditor.hpp"
#include "Utils/Log.hpp"
#include "Core/EntryPoint.hpp"
#include "EditorLayer.hpp"

sd::Application *sd::createApp() { return new SDEditor; }

void SDEditor::init() { pushLayer(new EditorLayer()); }

void SDEditor::destroy() {}
