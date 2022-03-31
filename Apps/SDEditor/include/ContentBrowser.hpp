#ifndef SD_CONTENT_BROWSER_HPP
#define SD_CONTENT_BROWSER_HPP

#include "Core/ECSSystem.hpp"
#include "Graphics/Texture.hpp"

namespace SD {

constexpr const char* DROP_ASSET_ITEM = "DROP_ASSET_ITEM";

class Asset;

class ContentBrowser : public ECSSystem {
   public:
    ContentBrowser();

    void OnInit() override;

    void OnImGui() override;

   private:

    void DrawModelCreation();
    void DrawTextureCreation();
    void DrawSceneCreation();
    void DrawFontCreation();

    bool m_open_creation{false};
    Ref<Texture> m_file_icon;
    Ref<Texture> m_directory_icon;

    std::filesystem::path m_current_directory;
};

}  // namespace SD
#endif
