#ifndef SD_EXPORT_HPP
#define SD_EXPORT_HPP

#include "Utility/Config.hpp"

#if defined(SD_UTILITY_EXPORT)
#define SD_UTILITY_API SD_API_EXPORT
#elif defined(SD_UTILITY_IMPORT)
#define SD_UTILITY_API SD_API_IMPORT
#else
#define SD_UTILITY_API
#endif

#endif
