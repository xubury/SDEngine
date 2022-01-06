#ifndef SD_ECS_EXPORT_HPP
#define SD_ECS_EXPORT_HPP

#include "Utility/Config.hpp"

#if defined(SD_ECS_EXPORT)
#define SD_ECS_API SD_API_EXPORT
#define ENTT_API_EXPORT
#elif defined(SD_ECS_IMPORT)
#define SD_ECS_API SD_API_IMPORT
#define ENTT_API_EXPORT
#else
#define SD_ECS_API
#endif

#endif
