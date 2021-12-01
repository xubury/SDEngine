#include "ImGui/FileDialog.hpp"

#include <chrono>
#include <string>
#include <filesystem>
#include <sstream>

using namespace std::chrono_literals;

typedef int ImGuiFileDialogSortOrder;

enum ImGuiFileDialogSortOrder_ {
    ImGuiFileDialogSortOrder_Up,
    ImGuiFileDialogSortOrder_Down,
    ImGuiFileDialogSortOrder_None
};

static void RefreshInfo(ImFileDialogInfo* dialogInfo) {
    dialogInfo->refresh_info = false;
    dialogInfo->current_index = 0;
    dialogInfo->current_files.clear();
    dialogInfo->current_directories.clear();

    for (const std::filesystem::directory_entry& entry :
         std::filesystem::directory_iterator(dialogInfo->directory_path)) {
        if (entry.is_directory()) {
            dialogInfo->current_directories.push_back(entry);
        } else {
            if (dialogInfo->file_extension.empty() ||
                entry.path().extension() == dialogInfo->file_extension) {
                dialogInfo->current_files.push_back(entry);
            }
        }
    }
}

bool ImGui::FileDialog(bool* open, ImFileDialogInfo* dialogInfo) {
    if (!*open) return false;

    static float initialSpacingColumn0 = 230.0f;
    static float initialSpacingColumn1 = 80.0f;
    static float initialSpacingColumn2 = 90.0f;
    static ImGuiFileDialogSortOrder fileNameSortOrder =
        ImGuiFileDialogSortOrder_None;
    static ImGuiFileDialogSortOrder sizeSortOrder =
        ImGuiFileDialogSortOrder_None;
    static ImGuiFileDialogSortOrder dateSortOrder =
        ImGuiFileDialogSortOrder_None;
    static ImGuiFileDialogSortOrder typeSortOrder =
        ImGuiFileDialogSortOrder_None;

    assert(dialogInfo != nullptr);

    bool complete = false;

    ImGui::PushID(dialogInfo);
    ImGui::SetNextWindowSize(ImVec2(740.0f, 410.0f), ImGuiCond_FirstUseEver);

    if (ImGui::Begin(dialogInfo->title.c_str(), open)) {
        if ((dialogInfo->current_files.empty() &&
             dialogInfo->current_directories.empty()) ||
            dialogInfo->refresh_info)
            RefreshInfo(dialogInfo);

        // Draw path
        ImGui::Text("Path: %s", dialogInfo->directory_path.string().c_str());

        ImGui::BeginChild("##browser",
                          ImVec2(ImGui::GetWindowContentRegionWidth(), 300),
                          true, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::Columns(4);

        // Columns size
        if (initialSpacingColumn0 > 0) {
            ImGui::SetColumnWidth(0, initialSpacingColumn0);
            initialSpacingColumn0 = 0.0f;
        }
        if (initialSpacingColumn1 > 0) {
            ImGui::SetColumnWidth(1, initialSpacingColumn1);
            initialSpacingColumn1 = 0.0f;
        }
        if (initialSpacingColumn2 > 0) {
            ImGui::SetColumnWidth(2, initialSpacingColumn2);
            initialSpacingColumn2 = 0.0f;
        }

        // File Columns
        if (ImGui::Selectable("Name")) {
            sizeSortOrder = ImGuiFileDialogSortOrder_None;
            dateSortOrder = ImGuiFileDialogSortOrder_None;
            typeSortOrder = ImGuiFileDialogSortOrder_None;
            fileNameSortOrder =
                fileNameSortOrder == ImGuiFileDialogSortOrder_Down
                    ? ImGuiFileDialogSortOrder_Up
                    : ImGuiFileDialogSortOrder_Down;
        }
        ImGui::NextColumn();
        if (ImGui::Selectable("Size")) {
            fileNameSortOrder = ImGuiFileDialogSortOrder_None;
            dateSortOrder = ImGuiFileDialogSortOrder_None;
            typeSortOrder = ImGuiFileDialogSortOrder_None;
            sizeSortOrder = sizeSortOrder == ImGuiFileDialogSortOrder_Down
                                ? ImGuiFileDialogSortOrder_Up
                                : ImGuiFileDialogSortOrder_Down;
        }
        ImGui::NextColumn();
        if (ImGui::Selectable("Type")) {
            fileNameSortOrder = ImGuiFileDialogSortOrder_None;
            dateSortOrder = ImGuiFileDialogSortOrder_None;
            sizeSortOrder = ImGuiFileDialogSortOrder_None;
            typeSortOrder = typeSortOrder == ImGuiFileDialogSortOrder_Down
                                ? ImGuiFileDialogSortOrder_Up
                                : ImGuiFileDialogSortOrder_Down;
        }
        ImGui::NextColumn();
        if (ImGui::Selectable("Date")) {
            fileNameSortOrder = ImGuiFileDialogSortOrder_None;
            sizeSortOrder = ImGuiFileDialogSortOrder_None;
            typeSortOrder = ImGuiFileDialogSortOrder_None;
            dateSortOrder = dateSortOrder == ImGuiFileDialogSortOrder_Down
                                ? ImGuiFileDialogSortOrder_Up
                                : ImGuiFileDialogSortOrder_Down;
        }
        ImGui::NextColumn();

        // File Separator
        ImGui::Separator();

        // Sort directories
        auto* directories = &dialogInfo->current_directories;

        if (fileNameSortOrder != ImGuiFileDialogSortOrder_None ||
            sizeSortOrder != ImGuiFileDialogSortOrder_None ||
            typeSortOrder != ImGuiFileDialogSortOrder_None) {
            std::sort(
                directories->begin(), directories->end(),
                [](const std::filesystem::directory_entry& a,
                   const std::filesystem::directory_entry& b) {
                    if (fileNameSortOrder == ImGuiFileDialogSortOrder_Down) {
                        return a.path().filename() > b.path().filename();
                    }

                    return a.path().filename() < b.path().filename();
                });
        } else if (dateSortOrder != ImGuiFileDialogSortOrder_None) {
            std::sort(directories->begin(), directories->end(),
                      [](const std::filesystem::directory_entry& a,
                         const std::filesystem::directory_entry& b) {
                          if (dateSortOrder == ImGuiFileDialogSortOrder_Down) {
                              return a.last_write_time() > b.last_write_time();
                          }
                          return a.last_write_time() < b.last_write_time();
                      });
        }

        // Sort files
        auto* files = &dialogInfo->current_files;

        if (fileNameSortOrder != ImGuiFileDialogSortOrder_None) {
            std::sort(
                files->begin(), files->end(),
                [](const std::filesystem::directory_entry& a,
                   const std::filesystem::directory_entry& b) {
                    if (fileNameSortOrder == ImGuiFileDialogSortOrder_Down) {
                        return a.path().filename() > b.path().filename();
                    }

                    return a.path().filename() < b.path().filename();
                });
        } else if (sizeSortOrder != ImGuiFileDialogSortOrder_None) {
            std::sort(files->begin(), files->end(),
                      [](const std::filesystem::directory_entry& a,
                         const std::filesystem::directory_entry& b) {
                          if (sizeSortOrder == ImGuiFileDialogSortOrder_Down) {
                              return a.file_size() > b.file_size();
                          }
                          return a.file_size() < b.file_size();
                      });
        } else if (typeSortOrder != ImGuiFileDialogSortOrder_None) {
            std::sort(files->begin(), files->end(),
                      [](const std::filesystem::directory_entry& a,
                         const std::filesystem::directory_entry& b) {
                          if (typeSortOrder == ImGuiFileDialogSortOrder_Down) {
                              return a.path().extension() >
                                     b.path().extension();
                          }

                          return a.path().extension() < b.path().extension();
                      });
        } else if (dateSortOrder != ImGuiFileDialogSortOrder_None) {
            std::sort(files->begin(), files->end(),
                      [](const std::filesystem::directory_entry& a,
                         const std::filesystem::directory_entry& b) {
                          if (dateSortOrder == ImGuiFileDialogSortOrder_Down) {
                              return a.last_write_time() > b.last_write_time();
                          }
                          return a.last_write_time() < b.last_write_time();
                      });
        }

        size_t index = 0;

        // Draw parent
        if (dialogInfo->directory_path.has_parent_path()) {
            if (ImGui::Selectable(
                    "..", dialogInfo->current_index == index,
                    ImGuiSelectableFlags_AllowDoubleClick,
                    ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
                dialogInfo->current_index = index;

                if (ImGui::IsMouseDoubleClicked(0)) {
                    dialogInfo->directory_path =
                        dialogInfo->directory_path.parent_path();
                    dialogInfo->refresh_info = true;
                }
            }
            ImGui::NextColumn();
            ImGui::TextUnformatted("-");
            ImGui::NextColumn();
            ImGui::TextUnformatted("<parent>");
            ImGui::NextColumn();
            ImGui::TextUnformatted("-");
            ImGui::NextColumn();

            index++;
        }

        // Draw directories
        for (size_t i = 0; i < directories->size(); ++i) {
            auto directoryEntry = dialogInfo->current_directories[i];
            auto directoryPath = directoryEntry.path();
            auto directoryName = directoryPath.filename();

            if (ImGui::Selectable(
                    directoryName.string().c_str(),
                    dialogInfo->current_index == index,
                    ImGuiSelectableFlags_AllowDoubleClick,
                    ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
                dialogInfo->current_index = index;

                if (ImGui::IsMouseDoubleClicked(0)) {
                    dialogInfo->directory_path = directoryPath;
                    dialogInfo->refresh_info = true;
                }
            }

            ImGui::NextColumn();
            ImGui::TextUnformatted("-");
            ImGui::NextColumn();
            ImGui::TextUnformatted("<directory>");
            ImGui::NextColumn();

            auto lastWriteTime = directoryEntry.last_write_time();
            auto st = std::chrono::time_point_cast<
                std::chrono::system_clock::duration>(
                lastWriteTime - decltype(lastWriteTime)::clock::now() +
                std::chrono::system_clock::now());
            std::time_t tt = std::chrono::system_clock::to_time_t(st);
            std::tm mt;
#if defined(SD_PLATFORM_WINDOWS)
            localtime_s(&mt, &tt);
#elif defined(SD_PLATFORM_LINUX)
            localtime_r(&tt, &mt);
#else
#error "Unknown platform!"
#endif
            std::stringstream ss;
            ss << std::put_time(&mt, "%Y-%m-%d %H:%M");
            ImGui::TextUnformatted(ss.str().c_str());
            ImGui::NextColumn();

            index++;
        }

        // Draw files
        for (size_t i = 0; i < files->size(); ++i) {
            auto fileEntry = dialogInfo->current_files[i];
            auto filePath = fileEntry.path();
            auto fileName = filePath.filename();

            if (ImGui::Selectable(
                    fileName.string().c_str(),
                    dialogInfo->current_index == index,
                    ImGuiSelectableFlags_AllowDoubleClick,
                    ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
                dialogInfo->current_index = index;
                dialogInfo->file_name = fileName;
            }

            ImGui::NextColumn();
            ImGui::TextUnformatted(
                std::to_string(fileEntry.file_size()).c_str());
            ImGui::NextColumn();
            ImGui::TextUnformatted(filePath.extension().string().c_str());
            ImGui::NextColumn();

            auto lastWriteTime = fileEntry.last_write_time();
            auto st = std::chrono::time_point_cast<
                std::chrono::system_clock::duration>(
                lastWriteTime - decltype(lastWriteTime)::clock::now() +
                std::chrono::system_clock::now());
            std::time_t tt = std::chrono::system_clock::to_time_t(st);
            std::tm mt;
#if defined(SD_PLATFORM_WINDOWS)
            localtime_s(&mt, &tt);
#elif defined(SD_PLATFORM_LINUX)
            localtime_r(&tt, &mt);
#else
#error "Unknown platform!"
#endif
            std::stringstream ss;
            ss << std::put_time(&mt, "%Y-%m-%d %H:%M");
            ImGui::TextUnformatted(ss.str().c_str());
            ImGui::NextColumn();

            index++;
        }
        ImGui::EndChild();

        // Draw filename
        static const size_t fileNameBufferSize = 200;
        static char fileNameBuffer[fileNameBufferSize];

        std::string fileNameStr = dialogInfo->file_name.string();
        size_t fileNameSize = fileNameStr.size();

        if (fileNameSize >= fileNameBufferSize)
            fileNameSize = fileNameBufferSize - 1;
        memcpy(fileNameBuffer, fileNameStr.c_str(), fileNameSize);
        fileNameBuffer[fileNameSize] = 0;

        ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth());
        if (ImGui::InputText("File Name", fileNameBuffer, fileNameBufferSize)) {
            dialogInfo->file_name = std::string(fileNameBuffer);
            dialogInfo->current_index = 0;
        }

        if (ImGui::Button("Cancel")) {
            fileNameSortOrder = ImGuiFileDialogSortOrder_None;
            sizeSortOrder = ImGuiFileDialogSortOrder_None;
            typeSortOrder = ImGuiFileDialogSortOrder_None;
            dateSortOrder = ImGuiFileDialogSortOrder_None;

            dialogInfo->refresh_info = false;
            dialogInfo->current_index = 0;
            dialogInfo->current_files.clear();
            dialogInfo->current_directories.clear();

            *open = false;
        }

        ImGui::SameLine();

        if (dialogInfo->type == ImGuiFileDialogType::OPEN_FILE) {
            if (ImGui::Button("Open")) {
                dialogInfo->result_path =
                    dialogInfo->directory_path / dialogInfo->file_name;

                if (std::filesystem::exists(dialogInfo->result_path)) {
                    fileNameSortOrder = ImGuiFileDialogSortOrder_None;
                    sizeSortOrder = ImGuiFileDialogSortOrder_None;
                    typeSortOrder = ImGuiFileDialogSortOrder_None;
                    dateSortOrder = ImGuiFileDialogSortOrder_None;

                    dialogInfo->refresh_info = false;
                    dialogInfo->current_index = 0;
                    dialogInfo->current_files.clear();
                    dialogInfo->current_directories.clear();

                    complete = true;
                    *open = false;
                }
            }
        } else if (dialogInfo->type == ImGuiFileDialogType::SAVE_FILE) {
            if (ImGui::Button("Save")) {
                dialogInfo->result_path =
                    dialogInfo->directory_path / dialogInfo->file_name;

                if (!std::filesystem::exists(dialogInfo->result_path)) {
                    fileNameSortOrder = ImGuiFileDialogSortOrder_None;
                    sizeSortOrder = ImGuiFileDialogSortOrder_None;
                    typeSortOrder = ImGuiFileDialogSortOrder_None;
                    dateSortOrder = ImGuiFileDialogSortOrder_None;

                    dialogInfo->refresh_info = false;
                    dialogInfo->current_index = 0;
                    dialogInfo->current_files.clear();
                    dialogInfo->current_directories.clear();

                    complete = true;
                    *open = false;
                }
            }
        }
    }

    ImGui::End();
    ImGui::PopID();

    return complete;
}
