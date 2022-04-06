#ifndef SD_EDITOR_HPP
#define SD_EDITOR_HPP

#include "Core/Application.hpp"
#include "EditorLayer.hpp"

namespace SD {

class SDEditor : public Application {
   public:
    SDEditor();
    void OnInit() override;
    void OnDestroy() override;

   private:
    EditorLayer* m_editor_layer;
};

}  // namespace SD

#endif /* SD_EDITOR_HPP */
