#ifndef SD_SYSTEM_EXPORT_HPP
#define SD_SYSTEM_EXPORT_HPP

#include "Utility/Config.hpp"

#if defined(SD_SYSTEM_EXPORT)
#define SD_SYSTEM_API SD_API_EXPORT
#elif defined(SD_SYSTEM_IMPORT)
#define SD_SYSTEM_API SD_API_IMPORT
#else
#define SD_SYSTEM_API
#endif

#endif
