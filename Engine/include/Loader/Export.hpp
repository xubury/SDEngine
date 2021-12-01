#ifndef SD_LOADER_EXPORT_HPP
#define SD_LOADER_EXPORT_HPP

#include "Utility/Config.hpp"

#ifdef SD_LOADER_EXPORT
#define SD_LOADER_API SD_API_EXPORT
#else
#define SD_LOADER_API SD_API_IMPORT
#endif

#endif
