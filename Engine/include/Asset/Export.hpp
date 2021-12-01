#ifndef SD_ASSET_EXPORT_HPP
#define SD_ASSET_EXPORT_HPP

#include "Utility/Config.hpp"

#ifdef SD_ASSET_EXPORT
#define SD_ASSET_API SD_API_EXPORT
#else
#define SD_ASSET_API SD_API_IMPORT
#endif

#endif
