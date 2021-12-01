#ifndef SD_GRAPHICS_EXPORT_HPP
#define SD_GRAPHICS_EXPORT_HPP

#include "Utility/Config.hpp"

#ifdef SD_GRAPHICS_EXPORT
#define SD_GRAPHICS_API SD_API_EXPORT
#else
#define SD_GRAPHICS_API SD_API_IMPORT
#endif

#endif
