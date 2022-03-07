#include "ContentBrowser.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGui/FileDialog.hpp"
#include "Asset/AssetStorage.hpp"

#include "Asset/ModelAsset.hpp"
#include "Asset/SceneAsset.hpp"
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

static void DrawModelCreation(const std::filesystem::path& directory,
                              bool* open) {
    if (open) {
        ImGui::OpenPopup("Setup asset property");
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});

        if (ImGui::BeginCenterPopupModal("Setup asset property")) {
            ImGui::BeginChild("##SHEET", ImVec2(300, 300));
            static bool filedlg_open = false;
            static ImFileDialogInfo filedlg_info;
            static char asset_name[255];
            ImGui::Text("Name");
            ImGui::Separator();
            ImGui::Columns();
            ImGui::InputText("##ASSET_FILE_NAME", asset_name,
                             sizeof(asset_name),
                             ImGuiInputTextFlags_EnterReturnsTrue);

            ImGui::Text("Model file");
            ImGui::Separator();
            ImGui::Columns(2, 0, false);
            ImGui::TextUnformatted(
                filedlg_info.result_path.generic_string().c_str());

            ImGui::NextColumn();
            if (ImGui::Button("Open")) {
                filedlg_open = true;
                filedlg_info.type = ImGuiFileDialogType::OPEN_FILE;
                filedlg_info.title = "Open File";
                // filedlg_info.regex_match = FONT_FILTER;
                filedlg_info.file_name = "";
                filedlg_info.directory_path =
                    AssetStorage::Get().GetDirectory();
            }
            ImGui::FileDialog(&filedlg_open, &filedlg_info);
            ImGui::EndChild();

            ImGui::Separator();
            ImGui::Columns();
            if (ImGui::Button("Confirm")) {
                auto asset = AssetStorage::Get().CreateAsset<ModelAsset>(
                    (directory / asset_name).generic_string());
                asset->SetModelPath(
                    filedlg_info.result_path.generic_string().c_str());
                asset->Init();
                *open = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                *open = false;
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::PopStyleVar();
        ImGui::EndPopup();
    }
}

void ContentBrowser::OnImGui() {
    ImGui::Begin("Content Browser");
    static bool create_new_asset = false;
    static std::function<void(bool*)> asset_creation_ui;
    // Right-click on blank space
    if (ImGui::BeginPopupContextWindow(0, 1, false)) {
        if (ImGui::MenuItem("Create Model Asset")) {
            create_new_asset = true;
            asset_creation_ui = std::bind(
                DrawModelCreation, m_current_directory, std::placeholders::_1);
        }

        if (ImGui::MenuItem("Create Scene Asset")) {
            create_new_asset = true;
        }
        ImGui::EndPopup();
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

    if (create_new_asset) {
        asset_creation_ui(&create_new_asset);
    }

    ImGui::SliderFloat("Thumbnail Size", &thumbnail_size, 16, 512);
    ImGui::SliderFloat("Padding", &padding, 0, 32);

    ImGui::End();
}

}  // namespace SD
