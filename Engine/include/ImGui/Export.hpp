#ifndef SD_IMGUI_EXPORT_HPP
#define SD_IMGUI_EXPORT_HPP

#include "Utility/Config.hpp"

#if defined(SD_IMGUI_EXPORT)
#define IMGUI_API SD_API_EXPORT
#elif defined(SD_IMGUI_IMPORT)
#define IMGUI_API SD_API_IMPORT
#else
#define IMGUI_API
#endif

#endif
