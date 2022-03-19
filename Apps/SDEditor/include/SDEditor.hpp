#ifndef SD_EDITOR_HPP
#define SD_EDITOR_HPP

#include "Core/Application.hpp"
#include "Renderer/GraphicsLayer.hpp"
#include "EditorLayer.hpp"

namespace SD {

class SDEditor : public Application {
   public:
    SDEditor();
    void OnInit() override;
    void OnDestroy() override;

    GraphicsLayer* GetGraphicsLayer() const { return m_graphics_layer; }

   private:
    EditorLayer* m_editor_layer;
    GraphicsLayer* m_graphics_layer;
};

}  // namespace SD

#endif /* SD_EDITOR_HPP */
