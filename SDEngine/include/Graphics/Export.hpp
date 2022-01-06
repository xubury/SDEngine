#ifndef SD_GRAPHICS_EXPORT_HPP
#define SD_GRAPHICS_EXPORT_HPP

#include "Utility/Config.hpp"

#if defined(SD_GRAPHICS_EXPORT)
#define SD_GRAPHICS_API SD_API_EXPORT
#elif defined(SD_GRAPHICS_IMPORT)
#define SD_GRAPHICS_API SD_API_IMPORT
#else
#define SD_GRAPHICS_API
#endif

#endif
