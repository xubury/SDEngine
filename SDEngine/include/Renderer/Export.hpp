#ifndef SD_RENDERER_EXPORT_HPP
#define SD_RENDERER_EXPORT_HPP

#include "Utility/Config.hpp"

#if defined(SD_RENDERER_EXPORT)
#define SD_RENDERER_API SD_API_EXPORT
#elif defined(SD_RENDERER_IMPORT)
#define SD_RENDERER_API SD_API_IMPORT
#else
#define SD_RENDERER_API
#endif

#endif
