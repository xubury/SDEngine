#ifndef SD_CORE_EXPORT_HPP
#define SD_CORE_EXPORT_HPP

#include "Utility/Config.hpp"

#if defined(SD_CORE_EXPORT)
#define SD_CORE_API SD_API_EXPORT
#elif defined(SD_CORE_IMPORT)
#define SD_CORE_API SD_API_IMPORT
#else
#define SD_CORE_API
#endif

#endif
