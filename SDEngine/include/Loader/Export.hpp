#ifndef SD_LOADER_EXPROT_HPP
#define SD_LOADER_EXPROT_HPP

#include "Utility/Config.hpp"

#if defined(SD_LOADER_EXPORT)
#define SD_LOADER_API SD_API_EXPORT
#elif defined(SD_LOADER_IMPORT)
#define SD_LOADER_API SD_API_IMPORT
#else
#define SD_LOADER_API
#endif

#endif
