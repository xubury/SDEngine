#ifndef SD_ECS_EXPORT_HPP
#define SD_ECS_EXPORT_HPP

#include "Utility/Config.hpp"

#ifdef SD_ECS_EXPORT
#define SD_ECS_API SD_API_EXPORT
#else
#define SD_ECS_API SD_API_IMPORT
#endif

#endif
