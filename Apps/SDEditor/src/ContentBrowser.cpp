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

template <typename T>
Asset* CreateAsset(const std::string& path) {
    AssetStorage& storage = AssetStorage::Get();
    T* asset = storage.CreateAsset<T>(path);
    return dynamic_cast<Asset*>(asset);
}

static void DrawModelCreation(Asset* asset) {
    ImGui::PushID(asset);
    ModelAsset* model_asset = dynamic_cast<ModelAsset*>(asset);
    if (model_asset) {
        ImGui::OpenPopup("Setup asset property");
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});

        if (ImGui::BeginCenterPopupModal("Setup asset property")) {
            ImGui::BeginChild("##SHEET", ImVec2(300, 300));

            ImGui::Text("Model file");
            ImGui::Separator();
            ImGui::Columns(2, 0, false);
            ImGui::TextUnformatted(model_asset->GetModelPath().c_str());

            static bool filedlg_open = false;
            static ImFileDialogInfo filedlg_info;
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
            if (ImGui::FileDialog(&filedlg_open, &filedlg_info)) {
                model_asset->SetModelPath(
                    filedlg_info.result_path.generic_string());
            }

            ImGui::EndChild();

            ImGui::Separator();
            ImGui::Columns();
            if (ImGui::Button("Confirm")) {
                model_asset->Init();
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                AssetStorage::Get().Unload<ModelAsset>(model_asset->GetId());
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::PopStyleVar();
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

void ContentBrowser::OnImGui() {
    ImGui::Begin("Content Browser");
    static bool create_new_asset = false;
    static std::function<Asset*(const std::string&)> create_func;
    static std::function<void(Asset*)> asset_creation_ui;
    // Right-click on blank space
    if (ImGui::BeginPopupContextWindow(0, 1, false)) {
        if (ImGui::MenuItem("Create Model Asset")) {
            create_new_asset = true;
            create_func =
                std::bind(&CreateAsset<ModelAsset>, std::placeholders::_1);
            asset_creation_ui =
                std::bind(DrawModelCreation, std::placeholders::_1);
        }

        if (ImGui::MenuItem("Create Scene Asset")) {
            create_new_asset = true;
            create_func =
                std::bind(&CreateAsset<SceneAsset>, std::placeholders::_1);
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

    static Asset* new_asset = nullptr;
    if (create_new_asset) {
        static char filename[255];
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::ImageButton((ImTextureID)(intptr_t)m_file_icon->GetId(),
                           {thumbnail_size, thumbnail_size});

        ImGui::PopStyleColor();
        if (ImGui::InputText("##ASSET_FILE_NAME", filename, sizeof(filename),
                             ImGuiInputTextFlags_EnterReturnsTrue)) {
            const std::filesystem::path full_path =
                m_current_directory / filename;
            if (!std::filesystem::exists(full_path)) {
                create_new_asset = false;
                new_asset = create_func(full_path.generic_string());
            } else {
                // File Already Exists
            }
        }
        ImGui::NextColumn();
    }
    ImGui::Columns(1);

    ImGui::SliderFloat("Thumbnail Size", &thumbnail_size, 16, 512);
    ImGui::SliderFloat("Padding", &padding, 0, 32);

    ImGui::End();

    if (new_asset != nullptr && !new_asset->IsInitialized()) {
        asset_creation_ui(new_asset);
    }
}

}  // namespace SD
