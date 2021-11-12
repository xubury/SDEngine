#ifndef IMGUI_FILE_DIALOG_HPP
#define IMGUI_FILE_DIALOG_HPP

#include "Utility/Base.hpp"
#include "imgui.h"

#include <filesystem>
#include <vector>
#include <algorithm>

typedef int ImGuiFileDialogType;  // -> enum ImGuiFileDialogType_        //
                                  // Enum: A file dialog type

enum ImGuiFileDialogType_ {
    ImGuiFileDialogType_OpenFile,
    ImGuiFileDialogType_SaveFile,
    ImGuiFileDialogType_COUNT
};

struct SD_API ImFileDialogInfo {
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
bool SD_API FileDialog(bool* open, ImFileDialogInfo* dialogInfo);
}

#endif
