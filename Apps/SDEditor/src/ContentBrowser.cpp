#include "ContentBrowser.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGui/FileDialog.hpp"
#include "Asset/AssetStorage.hpp"

#include "Asset/ModelAsset.hpp"
#include "Asset/SceneAsset.hpp"
#include "Asset/TextureAsset.hpp"

namespace SD {

ContentBrowser::ContentBrowser() : ECSSystem("Content Browser") {}

void ContentBrowser::OnInit()
{
    ECSSystem::OnInit();
    m_file_icon = TextureLoader::LoadTexture2D("assets/icons/FileIcon.png");
    m_directory_icon =
        TextureLoader::LoadTexture2D("assets/icons/DirectoryIcon.png");
    m_current_directory = AssetStorage::Get().GetDirectory();
}

static void DrawTextureCreation(const std::filesystem::path& directory,
                                bool* open)
{
    if (open) {
        auto& storage = AssetStorage::Get();
        ImGui::OpenPopup("Texture Creation");
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});

        if (ImGui::BeginCenterPopupModal("Texture Creation")) {
            ImGui::BeginChild("##SHEET", ImVec2(300, 300));
            ImGui::Columns(3, 0, false);
            static char asset_name[255];
            ImGui::Text("Name");
            ImGui::NextColumn();
            ImGui::InputText("##TEXTURE_ASSET_NAME", asset_name,
                             sizeof(asset_name));
            ImGui::NextColumn();
            ImGui::TextUnformatted(
                storage.GetTypeData(GetTypeId<TextureAsset>())
                    .file_extension.c_str());

            ImGui::NextColumn();
            ImGui::Text("Image File");

            ImGui::NextColumn();
            static ImFileDialogInfo filedlg_info;
            static bool filedlg_open = false;
            static std::string texture_path = "None";
            if (ImGui::Button(texture_path.c_str())) {
                filedlg_open = true;
                filedlg_info.type = ImGuiFileDialogType::OpenFile;
                filedlg_info.title = "Open File";
                // filedlg_info.regex_match = FONT_FILTER;
                filedlg_info.file_name = "";
                filedlg_info.directory_path = storage.GetDirectory();
            }
            if (ImGui::FileDialog(&filedlg_open, &filedlg_info)) {
                texture_path = filedlg_info.result_path.generic_string();
            }

            ImGui::EndChild();
            ImGui::Columns();

            ImGui::Separator();
            if (ImGui::Button("Confirm")) {
                auto asset = storage.CreateAsset<TextureAsset>(asset_name);
                asset->Import(texture_path);
                storage.SaveAsset(asset,
                                  (directory / asset_name).generic_string());

                ImGui::CloseCurrentPopup();
                *open = false;
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
                *open = false;
            }
        }
    }

    ImGui::PopStyleVar();
    ImGui::EndPopup();
}

static void DrawSceneCreation(const std::filesystem::path& directory,
                              bool* open)
{
    if (open) {
        ImGui::OpenPopup("Scene Creation");
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});

        auto& storage = AssetStorage::Get();
        if (ImGui::BeginCenterPopupModal("Scene Creation")) {
            ImGui::BeginChild("##SHEET", ImVec2(300, 300));
            ImGui::Columns(3, 0, false);
            static char asset_name[255];
            ImGui::Text("Name");
            ImGui::NextColumn();
            ImGui::InputText("##SCENE_ASSET_NAME", asset_name,
                             sizeof(asset_name));
            ImGui::NextColumn();
            ImGui::TextUnformatted(storage.GetTypeData(GetTypeId<SceneAsset>())
                                       .file_extension.c_str());

            ImGui::EndChild();
            ImGui::Columns();

            ImGui::Separator();
            if (ImGui::Button("Confirm")) {
                auto asset = storage.CreateAsset<SceneAsset>(asset_name);

                storage.SaveAsset(asset,
                                  (directory / asset_name).generic_string());
                ImGui::CloseCurrentPopup();
                *open = false;
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
                *open = false;
            }
        }
    }

    ImGui::PopStyleVar();
    ImGui::EndPopup();
}

static void DrawModelCreation(const std::filesystem::path& directory,
                              bool* open)
{
    if (open) {
        ImGui::OpenPopup("Model Creation");
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});

        auto& storage = AssetStorage::Get();
        if (ImGui::BeginCenterPopupModal("Model Creation")) {
            ImGui::BeginChild("##SHEET", ImVec2(300, 300));
            static char asset_name[255];
            ImGui::Columns(3, 0, false);

            ImGui::Text("Name");
            ImGui::NextColumn();
            ImGui::InputText("##MODEL_ASSET_NAME", asset_name,
                             sizeof(asset_name));
            ImGui::NextColumn();
            ImGui::TextUnformatted(storage.GetTypeData(GetTypeId<ModelAsset>())
                                       .file_extension.c_str());

            ImGui::NextColumn();
            ImGui::Text("Model file");

            ImGui::NextColumn();
            static ImFileDialogInfo filedlg_info;
            static bool filedlg_open = false;
            static std::string model_path = "None";
            if (ImGui::Button(model_path.c_str())) {
                filedlg_open = true;
                filedlg_info.type = ImGuiFileDialogType::OpenFile;
                filedlg_info.title = "Open File";
                // filedlg_info.regex_match = FONT_FILTER;
                filedlg_info.file_name = "";
                filedlg_info.directory_path = storage.GetDirectory();
            }
            if (ImGui::FileDialog(&filedlg_open, &filedlg_info)) {
                model_path = filedlg_info.result_path.generic_string();
            }

            ImGui::Columns();
            ImGui::EndChild();

            ImGui::Separator();
            if (ImGui::Button("Confirm")) {
                auto asset = storage.CreateAsset<ModelAsset>(asset_name);
                asset->Import(model_path);

                storage.SaveAsset(asset,
                                  (directory / asset_name).generic_string());
                ImGui::CloseCurrentPopup();
                *open = false;
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
                *open = false;
            }
        }

        ImGui::PopStyleVar();
        ImGui::EndPopup();
    }
}

void ContentBrowser::OnImGui()
{
    auto& storage = AssetStorage::Get();
    ImGui::Begin("Content Browser");
    static bool create_new_asset = false;
    static std::function<void(bool*)> asset_creation_ui;
    // Right-click on blank space
    if (ImGui::BeginPopupContextWindow(0, 1, false)) {
        auto menu_item =
            [&](const std::string& desc,
                const std::function<void(const std::filesystem::path& path,
                                         bool*)>& func) {
                if (ImGui::MenuItem(desc.c_str())) {
                    create_new_asset = true;
                    asset_creation_ui = std::bind(func, m_current_directory,
                                                  std::placeholders::_1);
                }
            };
        menu_item("Create Model Asset", DrawModelCreation);
        menu_item("Create Scene Asset", DrawSceneCreation);
        menu_item("Create Texture Asset", DrawTextureCreation);
        ImGui::EndPopup();
    }

    const std::filesystem::path root_path = storage.GetDirectory();
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
        ImGui::ImageButton((ImTextureID)(intptr_t)icon->GetId(),
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

    if (create_new_asset) {
        asset_creation_ui(&create_new_asset);
    }

    ImGui::SliderFloat("Thumbnail Size", &thumbnail_size, 16, 512);
    ImGui::SliderFloat("Padding", &padding, 0, 32);

    ImGui::End();
}

}  // namespace SD
