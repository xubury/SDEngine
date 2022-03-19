#ifndef IMGUI_FILE_DIALOG_HPP
#define IMGUI_FILE_DIALOG_HPP

#include "Utility/Base.hpp"
#include "ImGui/Export.hpp"
#include "imgui.h"

#include <filesystem>
#include <vector>
#include <algorithm>
#include <regex>

enum class ImGuiFileDialogType { OpenFile, SaveFile, TypeCount = 2 };

// TODO: model filter?
const std::regex IMG_FILTER("(.*\\.jpg)|(.*\\.png)|(.*\\.tif)|(.*\\.bmp)");
const std::regex SCENE_FILTER(".*\\.scene");
const std::regex FONT_FILTER(".*\\.font");

struct IMGUI_API ImFileDialogInfo {
    std::string title{"File Dialog"};
    std::regex regex_match{"(.*)"};
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
bool IMGUI_API FileDialog(bool* open, ImFileDialogInfo* dialogInfo);
}

#endif
