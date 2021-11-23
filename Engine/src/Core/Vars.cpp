#include "Core/Vars.hpp"

#if defined(SD_PLATFORM_LINUX)
#include <unistd.h>
#endif

#if defined(SD_PLATFORM_WINDOWS)
#include <windows.h>
#endif

namespace SD {

std::filesystem::path GetAppDirectory() {
    static std::filesystem::path path;
    if (path.empty()) {
#if defined(SD_PLATFORM_LINUX)
        char buffer[260];
        size_t size = readlink("/proc/self/exe", buffer, sizeof(buffer));
        if (size > 0) {
            path.assign(std::begin(buffer), std::begin(buffer) + size);
        }
#elif defined(SD_PLATFORM_WINDOWS)
        char buffer[MAX_PATH];
        size_t size = GetModuleFileNameA(NULL, buffer, MAX_PATH);
        if (size > 0) {
            path.assign(std::begin(buffer), std::begin(buffer) + size);
        }
#endif
    }
    return path.parent_path();
}

}  // namespace SD
