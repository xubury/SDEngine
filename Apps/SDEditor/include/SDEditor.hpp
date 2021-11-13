#ifndef SD_EDITOR_HPP
#define SD_EDITOR_HPP

#include "Core/Application.hpp"
#include "EditorLayer.hpp"

namespace SD {

class SDEditor : public Application {
   public:
    void onInit() override;
};

}  // namespace SD

#endif /* SD_EDITOR_HPP */
