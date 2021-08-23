#include "SDEditor.hpp"
#include "Common/Log.hpp"
#include "Core/EntryPoint.hpp"
#include "EditorLayer.hpp"

sd::Application *sd::createApp() { return new SDEditor; }

void SDEditor::init() { pushLayer(new EditorLayer()); }

void SDEditor::destroy() {}
