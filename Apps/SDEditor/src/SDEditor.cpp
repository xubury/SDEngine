#include "SDEditor.hpp"

#include "Core/EntryPoint.hpp"
#include "EditorLayer.hpp"
#include "Utility/Log.hpp"

IMPLEMENT_APP(SDEditor);

void SDEditor::init() { pushLayer(sd::createRef<EditorLayer>(800, 600)); }

void SDEditor::destroy() {}
