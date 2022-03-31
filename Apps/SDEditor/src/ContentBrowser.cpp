#include "ContentBrowser.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGui/FileDialog.hpp"
#include "Asset/AssetStorage.hpp"

#include "Asset/ModelAsset.hpp"
#include "Asset/SceneAsset.hpp"
#include "Asset/TextureAsset.hpp"
#include "Asset/FontAsset.hpp"

namespace SD {

#define MODEL_CREATION "Model Creation"
#define SCENE_CREATION "Scene Creation"
#define TEXTURE_CREATION "Texture Creation"
#define FONT_CREATION "Font Creation"

ContentBrowser::ContentBrowser() : ECSSystem("Content Browser") {}

void ContentBrowser::OnInit()
{
    ECSSystem::OnInit();
    m_file_icon = TextureLoader::LoadTexture2D("assets/icons/FileIcon.png");
    m_directory_icon =
        TextureLoader::LoadTexture2D("assets/icons/DirectoryIcon.png");
    m_current_directory = AssetStorage::Get().GetDirectory();
}

void ContentBrowser::DrawTextureCreation()
{
    auto& storage = AssetStorage::Get();
    if (ImGui::BeginCenterPopupModal(TEXTURE_CREATION, &m_open_creation)) {
        ImGui::BeginChild("##SHEET", ImVec2(300, 300));
        ImGui::Columns(3, 0, false);
        static char asset_name[255];
        ImGui::Text("Name");
        ImGui::NextColumn();
        ImGui::InputText("##TEXTURE_ASSET_NAME", asset_name,
                         sizeof(asset_name));
        ImGui::NextColumn();
        ImGui::TextUnformatted(
            storage.GetTypeData<TextureAsset>().file_extension.c_str());

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
            storage.SaveAsset(
                asset, (m_current_directory / asset_name).generic_string());

            ImGui::CloseCurrentPopup();
            m_open_creation = false;
        }

        ImGui::EndPopup();
    }
}

void ContentBrowser::DrawSceneCreation()
{
    auto& storage = AssetStorage::Get();
    if (ImGui::BeginCenterPopupModal(SCENE_CREATION, &m_open_creation)) {
        ImGui::BeginChild("##SHEET", ImVec2(300, 300));
        ImGui::Columns(3, 0, false);
        static char asset_name[255];
        ImGui::Text("Name");
        ImGui::NextColumn();
        ImGui::InputText("##SCENE_ASSET_NAME", asset_name, sizeof(asset_name));
        ImGui::NextColumn();
        ImGui::TextUnformatted(
            storage.GetTypeData<SceneAsset>().file_extension.c_str());

        ImGui::EndChild();
        ImGui::Columns();

        ImGui::Separator();
        if (ImGui::Button("Confirm")) {
            auto asset = storage.CreateAsset<SceneAsset>(asset_name);

            storage.SaveAsset(
                asset, (m_current_directory / asset_name).generic_string());
            ImGui::CloseCurrentPopup();
            m_open_creation = false;
        }

        ImGui::EndPopup();
    }
}

void ContentBrowser::DrawModelCreation()
{
    auto& storage = AssetStorage::Get();
    if (ImGui::BeginCenterPopupModal(MODEL_CREATION, &m_open_creation)) {
        ImGui::BeginChild("##SHEET", ImVec2(300, 300));
        static char asset_name[255];
        ImGui::Columns(3, 0, false);

        ImGui::Text("Name");
        ImGui::NextColumn();
        ImGui::InputText("##MODEL_ASSET_NAME", asset_name, sizeof(asset_name));
        ImGui::NextColumn();
        ImGui::TextUnformatted(
            storage.GetTypeData<ModelAsset>().file_extension.c_str());

        ImGui::NextColumn();
        ImGui::Text("Model File");

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

            storage.SaveAsset(
                asset, (m_current_directory / asset_name).generic_string());
            ImGui::CloseCurrentPopup();
            m_open_creation = false;
        }

        ImGui::EndPopup();
    }
}

void ContentBrowser::DrawFontCreation()
{
    auto& storage = AssetStorage::Get();
    if (ImGui::BeginCenterPopupModal(FONT_CREATION, &m_open_creation)) {
        ImGui::BeginChild("##SHEET", ImVec2(300, 300));
        ImGui::Columns(3, 0, false);
        static char asset_name[255];
        ImGui::TextUnformatted("Name");
        ImGui::NextColumn();
        ImGui::InputText("##FONT_ASSET_NAME", asset_name, sizeof(asset_name));
        ImGui::NextColumn();
        ImGui::TextUnformatted(
            storage.GetTypeData<FontAsset>().file_extension.c_str());
        ImGui::NextColumn();

        static int32_t font_height = 20;
        ImGui::TextUnformatted("Font Height");
        ImGui::NextColumn();
        ImGui::InputInt("##FONT_HEIGHT", &font_height);
        ImGui::NextColumn();
        ImGui::TextUnformatted("px");
        ImGui::NextColumn();

        ImGui::NextColumn();
        ImGui::Text("Font File");

        static ImFileDialogInfo filedlg_info;
        static bool filedlg_open = false;
        static std::string font_path = "None";
        if (ImGui::Button(font_path.c_str())) {
            filedlg_open = true;
            filedlg_info.type = ImGuiFileDialogType::OpenFile;
            filedlg_info.title = "Open File";
            // filedlg_info.regex_match = FONT_FILTER;
            filedlg_info.file_name = "";
            filedlg_info.directory_path = storage.GetDirectory();
        }
        if (ImGui::FileDialog(&filedlg_open, &filedlg_info)) {
            font_path = filedlg_info.result_path.generic_string();
        }

        ImGui::EndChild();
        ImGui::Columns();

        ImGui::Separator();
        if (ImGui::Button("Confirm")) {
            auto asset = storage.CreateAsset<FontAsset>(asset_name);
            asset->Import(font_path, font_height);

            storage.SaveAsset(
                asset, (m_current_directory / asset_name).generic_string());
            ImGui::CloseCurrentPopup();
            m_open_creation = false;
        }

        ImGui::EndPopup();
    }
}

void ContentBrowser::OnImGui()
{
    auto& storage = AssetStorage::Get();
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

    if (m_open_creation) {
        ImGui::OpenPopup(creation_str);
    }
    DrawTextureCreation();
    DrawModelCreation();
    DrawSceneCreation();
    DrawFontCreation();

    ImGui::SliderFloat("Thumbnail Size", &thumbnail_size, 16, 512);
    ImGui::SliderFloat("Padding", &padding, 0, 32);

    ImGui::End();
}

}  // namespace SD
