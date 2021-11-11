#include "SDEditor.hpp"

#include "Core/EntryPoint.hpp"
#include "EditorLayer.hpp"
#include "Utility/Log.hpp"

IMPLEMENT_APP(SDEditor);

namespace SD {

void SDEditor::init() {
    m_layer = dynamic_cast<EditorLayer *>(pushLayer(new EditorLayer(800, 600)));
}

void SDEditor::destroy() { delete m_layer; }

}  // namespace SD
