#ifndef SD_EDITOR_HPP
#define SD_EDITOR_HPP

#include "Core/Application.hpp"
#include "EditorLayer.hpp"

namespace SD {

class SDEditor : public Application {
   public:
    void init() override;
    void destroy() override;

   private:
    EditorLayer *m_layer;
};

}  // namespace SD

#endif /* SD_EDITOR_HPP */
