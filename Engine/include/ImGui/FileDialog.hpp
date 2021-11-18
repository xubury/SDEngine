#ifndef IMGUI_FILE_DIALOG_HPP
#define IMGUI_FILE_DIALOG_HPP

#include "Utility/Base.hpp"
#include "imgui.h"

#include <filesystem>
#include <vector>
#include <algorithm>

enum class ImGuiFileDialogType {
    OPEN_FILE,
    SAVE_FILE,
    TYPE_COUNT = 2
};

struct SD_API ImFileDialogInfo {
    std::string title;
    std::filesystem::path file_extension;
    ImGuiFileDialogType type;

    std::filesystem::path file_name;
    std::filesystem::path directory_path;
    std::filesystem::path result_path;

    bool refresh_info;
    size_t current_index;
    std::vector<std::filesystem::directory_entry> current_files;
    std::vector<std::filesystem::directory_entry> current_directories;
};

namespace ImGui {
bool SD_API FileDialog(bool* open, ImFileDialogInfo* dialogInfo);
}

#endif
