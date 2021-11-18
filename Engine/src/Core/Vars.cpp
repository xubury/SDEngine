#include "Core/Vars.hpp"

#if defined(SD_PLATFORM_LINUX)
#include <unistd.h>
#endif

namespace SD {

std::filesystem::path GetAppDirectory() {
    std::filesystem::path path;
#if defined(SD_PLATFORM_LINUX)
    char buffer[260];
    size_t size = readlink("/proc/self/exe", buffer, sizeof(buffer));
    if (size > 0) {
        path.assign(std::begin(buffer), std::begin(buffer) + size);
    }
#elif defined(SD_PLATFORM_WINDOW)
#error "not implemented yet"
#endif
    return path.parent_path();
}

}  // namespace SD
