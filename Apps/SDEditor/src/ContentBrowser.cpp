#include "ContentBrowser.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGui/FileDialog.hpp"
#include "Resource/ResourceManager.hpp"
#include "Loader/ImageLoader.hpp"

namespace SD {

#define MODEL_CREATION "Model Creation"
#define SCENE_CREATION "Scene Creation"
#define TEXTURE_CREATION "Texture Creation"
#define FONT_CREATION "Font Creation"

ContentBrowser::ContentBrowser()
{
    auto& resource = ResourceManager::Get();
    m_file_icon = Texture::CreateIcon(
        *ImageLoader::LoadFromFile("assets/icons/FileIcon.png"));
    m_directory_icon = Texture::CreateIcon(
        *ImageLoader::LoadFromFile("assets/icons/DirectoryIcon.png"));
    m_current_directory = resource.GetDirectory();
}

void ContentBrowser::ImGui()
{
    ImGui::Begin("Content Browser");
    static const char* creation_str = nullptr;
    // Right-click on blank space
    if (ImGui::BeginPopupContextWindow(0, 1, false)) {
        if (ImGui::MenuItem("Create Texture Asset")) {
            m_open_creation = true;
            creation_str = TEXTURE_CREATION;
        }
        if (ImGui::MenuItem("Create Model Asset")) {
            m_open_creation = true;
            creation_str = MODEL_CREATION;
        }
        if (ImGui::MenuItem("Create Scene Asset")) {
            m_open_creation = true;
            creation_str = SCENE_CREATION;
        }

        if (ImGui::MenuItem("Create Font Asset")) {
            m_open_creation = true;
            creation_str = FONT_CREATION;
        }
        ImGui::EndPopup();
    }

    auto& resource = ResourceManager::Get();
    const std::filesystem::path root_path = resource.GetDirectory();
    if (m_current_directory != std::filesystem::path(root_path)) {
        if (ImGui::Button("<-")) {
            m_current_directory = m_current_directory.parent_path();
        }
    }

    static float padding = 16.0f;
    static float thumbnail_size = 80.0f;
    const float cell_size = thumbnail_size + padding;

    const float panel_width = ImGui::GetContentRegionAvail().x;
    int column_cnt = std::floor(panel_width / cell_size);
    if (column_cnt < 1) column_cnt = 1;

    ImGui::Columns(column_cnt, 0, false);

    for (auto& entry :
         std::filesystem::directory_iterator(m_current_directory)) {
        const auto& path = entry.path();
        const bool is_directory = entry.is_directory();
        const std::string filename = path.filename().string();

        Ref<Texture> icon = is_directory ? m_directory_icon : m_file_icon;
        ImGui::PushID(filename.c_str());
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::ImageButton((ImTextureID)(intptr_t)icon->Handle(),
                           {thumbnail_size, thumbnail_size});

        if (!is_directory && ImGui::BeginDragDropSource()) {
            const std::string item_path = path.generic_string();
            ImGui::SetDragDropPayload(DROP_ASSET_ITEM, item_path.c_str(),
                                      item_path.size() + 1);
            ImGui::TextUnformatted(item_path.c_str());
            ImGui::EndDragDropSource();
        }

        ImGui::PopStyleColor();
        if (ImGui::IsItemHovered() &&
            ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            if (entry.is_directory()) m_current_directory /= path.filename();
        }
        ImGui::TextWrapped("%s", filename.c_str());

        ImGui::NextColumn();

        ImGui::PopID();
    }

    ImGui::Columns(1);

    if (m_open_creation) {
        ImGui::OpenPopup(creation_str);
    }
    // DrawTextureCreation();
    // DrawModelCreation();
    // DrawSceneCreation();
    // DrawFontCreation();

    ImGui::SliderFloat("Thumbnail Size", &thumbnail_size, 16, 512);
    ImGui::SliderFloat("Padding", &padding, 0, 32);

    ImGui::End();
}

}  // namespace SD
