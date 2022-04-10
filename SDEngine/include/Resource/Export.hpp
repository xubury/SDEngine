#ifndef SD_RESOURCE_EXPROT_HPP
#define SD_RESOURCE_EXPROT_HPP

#include "Utility/Config.hpp"

#if defined(SD_RESOURCE_EXPORT)
#define SD_RESOURCE_API SD_API_EXPORT
#elif defined(SD_RESOURCE_IMPORT)
#define SD_RESOURCE_API SD_API_IMPORT
#else
#define SD_RESOURCE_API
#endif

#endif
