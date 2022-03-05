#include "ContentBrowser.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "Loader/TextureLoader.hpp"
#include "Asset/AssetStorage.hpp"

#include "Asset/ModelAsset.hpp"
#include "Asset/TextureAsset.hpp"

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
    AssetStorage& storage = AssetStorage::Get();
    ImGui::Begin("Content Browser");
    static bool create_asset = false;
    // Right-click on blank space
    if (ImGui::BeginPopupContextWindow(0, 1, false)) {
        if (ImGui::MenuItem("Create model asset")) {
            create_asset = true;
        }

        ImGui::EndPopup();
    }
    if (create_asset) {
        ImGui::OpenPopup("Model asset creation");
        if (ImGui::BeginCenterPopupModal("Model asset creation")) {
            ImGui::TextUnformatted("Create model asset:");
            static char name[255];
            ImGui::InputText("Model Name: ", name, sizeof(name));
            static std::string path =
                (storage.GetDirectory() / "models/Sponza/sponza.obj")
                    .generic_string();
            ImGui::InputText("Model File: ", path.data(), path.size() + 1);
            if (ImGui::Button("Ok")) {
                ModelAsset* asset =
                    storage.CreateAsset<ModelAsset>(m_current_directory / name);
                asset->SetModelPath(path);
                asset->Init();

                create_asset = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("No")) {
                create_asset = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    const std::filesystem::path root_path = AssetStorage::Get().GetDirectory();
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
        const std::filesystem::path relative_path =
            std::filesystem::relative(path, root_path);
        const std::string filename = relative_path.filename().string();

        Ref<Texture> icon = is_directory ? m_directory_icon : m_file_icon;
        ImGui::PushID(filename.c_str());
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
