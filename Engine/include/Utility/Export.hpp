#ifndef SD_EXPORT_HPP
#define SD_EXPORT_HPP

#include "Utility/Config.hpp"

#ifdef SD_UTILITY_EXPORT
#define SD_UTILITY_API SD_API_EXPORT
#else
#define SD_UTILITY_API SD_API_IMPORT
#endif

#endif
