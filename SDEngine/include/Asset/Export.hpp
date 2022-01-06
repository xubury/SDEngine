#ifndef SD_ASSET_EXPORT_HPP
#define SD_ASSET_EXPORT_HPP

#include "Utility/Config.hpp"

#if defined(SD_ASSET_EXPORT)
#define SD_ASSET_API SD_API_EXPORT
#elif defined(SD_ASSET_IMPORT)
#define SD_ASSET_API SD_API_IMPORT
#else
#define SD_ASSET_API
#endif

#endif
