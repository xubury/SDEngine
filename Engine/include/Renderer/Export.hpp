#ifndef SD_RENDERER_EXPORT_HPP
#define SD_RENDERER_EXPORT_HPP

#include "Utility/Config.hpp"

#ifdef SD_RENDERER_EXPORT
#define SD_RENDERER_API SD_API_EXPORT
#else
#define SD_RENDERER_API SD_API_IMPORT
#endif

#endif
