#ifndef SD_EXPORT_HPP
#define SD_EXPORT_HPP

#include "Core/Config.hpp"

#ifdef SD_EXPORT
#define SD_API SD_API_EXPORT
#else
#define SD_API SD_API_IMPORT
#endif

#endif
