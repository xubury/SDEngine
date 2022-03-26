#ifndef SD_SCRIPT_EXPORT_HPP
#define SD_SCRIPT_EXPORT_HPP

#include "Utility/Config.hpp"

#if defined(SD_SCRIPT_EXPORT)
#define SD_SCRIPT_API SD_API_EXPORT
#elif defined(SD_SCRIPT_IMPORT)
#define SD_SCRIPT_API SD_API_IMPORT
#else
#define SD_SCRIPT_API
#endif

#endif
