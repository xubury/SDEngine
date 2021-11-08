#include "SDEditor.hpp"

#include "Core/EntryPoint.hpp"
#include "EditorLayer.hpp"
#include "Utility/Log.hpp"

IMPLEMENT_APP(SDEditor);

namespace sd {

void SDEditor::init() { pushLayer(createRef<EditorLayer>(800, 600)); }

void SDEditor::destroy() {}

}  // namespace sd
