#ifndef IMGUI_FILE_DIALOG_HPP
#define IMGUI_FILE_DIALOG_HPP
#include <filesystem>
#include <imgui.h>
#include <vector>
#include <algorithm>

typedef int ImGuiFileDialogType;  // -> enum ImGuiFileDialogType_        //
                                  // Enum: A file dialog type

enum ImGuiFileDialogType_ {
    ImGuiFileDialogType_OpenFile,
    ImGuiFileDialogType_SaveFile,
    ImGuiFileDialogType_COUNT
};

struct ImFileDialogInfo {
    std::string title;
    std::filesystem::path fileExtension;
    ImGuiFileDialogType type;

    std::filesystem::path fileName;
    std::filesystem::path directoryPath;
    std::filesystem::path resultPath;

    bool refreshInfo;
    size_t currentIndex;
    std::vector<std::filesystem::directory_entry> currentFiles;
    std::vector<std::filesystem::directory_entry> currentDirectories;
};

namespace ImGui {
IMGUI_API bool FileDialog(bool* open, ImFileDialogInfo* dialogInfo);
}

#endif
