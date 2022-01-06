#ifndef SD_TILE_MAP_EXPORT_HPP
#define SD_TILE_MAP_EXPORT_HPP

#include "Utility/Config.hpp"

#if defined(SD_TILE_MAP_EXPORT)
#define SD_TILE_MAP_API SD_API_EXPORT
#elif defined(SD_TILE_MAP_IMPORT)
#define SD_TILE_MAP_API SD_API_IMPORT
#else
#define SD_TILE_MAP_API
#endif

#endif
