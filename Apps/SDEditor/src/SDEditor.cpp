#include "SDEditor.hpp"

#include "Core/EntryPoint.hpp"
#include "EditorLayer.hpp"

IMPLEMENT_APP(SDEditor);

namespace SD {

void SDEditor::onInit() { pushLayer(new EditorLayer(800, 600)); }

}  // namespace SD
