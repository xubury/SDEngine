#ifndef SD_RESOURCE_LOADER_HPP
#define SD_RESOURCE_LOADER_HPP

#include "Resource/ResourceHandle.hpp"

namespace SD {

template <typename Loader, typename Resource>
class ResourceLoader {
   public:
    template <typename... Args>
    [[nodiscard]] ResourceHandle<Resource> Get(Args&&... args) const
    {
        return static_cast<const Loader*>(this)->Load(
            std::forward<Args>(args)...);
    }
};

}  // namespace SD

#endif  // !SD_RESOURCE_LOADER_HPP
