#include "ContentBrowser.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "Loader/TextureLoader.hpp"
#include "Asset/AssetStorage.hpp"

namespace SD {

ContentBrowser::ContentBrowser() : System("Content Browser") {}

void ContentBrowser::OnInit() {
    System::OnInit();
    m_file_icon = TextureLoader::LoadTexture2D("assets/icons/FileIcon.png");
    m_directory_icon =
        TextureLoader::LoadTexture2D("assets/icons/DirectoryIcon.png");
    m_current_directory = AssetStorage::Get().GetDirectory();
}

void ContentBrowser::OnImGui() {
    ImGui::Begin("Content Browser");
    const std::filesystem::path root_path = AssetStorage::Get().GetDirectory();
    if (m_current_directory != std::filesystem::path(root_path)) {
        if (ImGui::Button("<-")) {
            m_current_directory = m_current_directory.parent_path();
        }
    }

    static float padding = 16.0f;
    static float thumbnail_size = 128.0f;
    float cell_size = thumbnail_size + padding;

    float panel_width = ImGui::GetContentRegionAvail().x;
    int column_cnt = (int)(panel_width / cell_size);
    if (column_cnt < 1) column_cnt = 1;

    ImGui::Columns(column_cnt, 0, false);

    for (auto& entry :
         std::filesystem::directory_iterator(m_current_directory)) {
        const auto& path = entry.path();
        const std::filesystem::path relative_path =
            std::filesystem::relative(path, root_path);
        const std::string filename = relative_path.filename().string();

        ImGui::PushID(filename.c_str());
        Ref<Texture> icon =
            entry.is_directory() ? m_directory_icon : m_file_icon;
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::ImageButton((ImTextureID)(intptr_t)icon->GetId(),
                           {thumbnail_size, thumbnail_size});

        if (ImGui::BeginDragDropSource()) {
            const std::string item_path = relative_path.string();
            ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", item_path.c_str(),
                                      item_path.size() + 1);
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

    ImGui::SliderFloat("Thumbnail Size", &thumbnail_size, 16, 512);
    ImGui::SliderFloat("Padding", &padding, 0, 32);

    ImGui::End();
}

}  // namespace SD
