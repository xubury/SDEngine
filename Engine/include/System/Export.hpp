#ifndef SD_SYSTEM_EXPORT_HPP
#define SD_SYSTEM_EXPORT_HPP

#include "Utility/Config.hpp"

#ifdef SD_SYSTEM_EXPORT
#define SD_SYSTEM_API SD_API_EXPORT
#else
#define SD_SYSTEM_API SD_API_IMPORT
#endif

#endif
