#ifndef SD_CONTENT_BROWSER_HPP
#define SD_CONTENT_BROWSER_HPP

#include "Core/System.hpp"

namespace SD {

constexpr const char* DROP_ASSET_ITEM = "DROP_ASSET_ITEM";

class Asset;

class ContentBrowser : public System {
   public:
    ContentBrowser();

    void OnInit() override;

    void OnImGui() override;

   private:
    Ref<Texture> m_file_icon;
    Ref<Texture> m_directory_icon;

    std::filesystem::path m_current_directory;
};

}  // namespace SD
#endif
