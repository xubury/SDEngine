#ifndef SD_CORE_EXPORT_HPP
#define SD_CORE_EXPORT_HPP

#include "Utility/Config.hpp"

#ifdef SD_CORE_EXPORT
#define SD_CORE_API SD_API_EXPORT
#else
#define SD_CORE_API SD_API_IMPORT
#endif

#endif
