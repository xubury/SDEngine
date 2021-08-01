#ifndef SD_CONFIG_HPP
#define SD_CONFIG_HPP

#include "Utils/PlatformDetection.hpp"

#if defined(SD_SHARED)

#if defined(SD_PLATFORM_WINDOWS)

// Windows compilers need specific (and different) keywords for export and
// import
#define SD_API_EXPORT __declspec(dllexport)
#define SD_API_IMPORT __declspec(dllimport)

// For Visual C++ compilers, we also need to turn off this annoying C4251
// warning
#ifdef _MSC_VER

#pragma warning(disable : 4251)

#endif

#else  // Linux, FreeBSD, Mac OS X

#if __GNUC__ >= 4

// GCC 4 has special keywords for showing/hidding symbols,
// the same keyword is used for both importing and exporting
#define SD_API_EXPORT __attribute__((__visibility__("default")))
#define SD_API_IMPORT __attribute__((__visibility__("default")))

#else

// GCC < 4 has no mechanism to explicitely hide symbols, everything's exported
#define SD_API_EXPORT
#define SD_API_IMPORT

#endif

#endif

#else

// Static build doesn't need import/export macros
#define SD_API_EXPORT
#define SD_API_IMPORT

#endif

#endif
