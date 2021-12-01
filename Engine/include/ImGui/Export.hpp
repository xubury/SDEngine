#ifndef SD_IMGUI_EXPORT_HPP
#define SD_IMGUI_EXPORT_HPP

#include "Utility/Config.hpp"

#ifdef SD_IMGUI_EXPORT
#define IMGUI_API SD_API_EXPORT
#else
#define IMGUI_API SD_API_IMPORT
#endif

#endif
