#ifndef SD_RESOURCE_CACHE_HPP
#define SD_RESOURCE_CACHE_HPP

#include "Resource/ResourceId.hpp"
#include "Resource/ResourceHandle.hpp"
#include <unordered_map>
#include <cstdint>

namespace SD {

template <typename Resource>
class ResourceCache {
   public:
    template <typename Loader, typename... Args>
    [[nodiscard]] ResourceHandle<Resource> Load(const ResourceId rid,
                                                Args &&...args)
    {
        if (auto it = m_resources.find(rid); it == m_resources.cend()) {
            if (auto handle = Temp<Loader>(std::forward<Args>(args)...);
                handle) {
                return m_resources[rid] = std::move(handle);
            }
        }
        else {
            return it->second;
        }

        return {};
    }

    template <typename Loader, typename... Args>
    [[nodiscard]] ResourceHandle<Resource> Temp(Args &&...args)
    {
        return Loader{}.Get(std::forward<Args>(args)...);
    }

    void Discard(const ResourceId rid)
    {
        if (auto it = m_resources.find(rid); it != m_resources.end()) {
            m_resources.erase(it);
        }
    }

    template <typename Loader, typename... Args>
    [[nodiscard]] ResourceHandle<Resource> Reload(const ResourceId rid,
                                                  Args &&...args)
    {
        return (Discard(rid), Load<Loader>(rid, std::forward<Args>(args)...));
    }

    [[nodiscard]] ResourceHandle<Resource> Handle(const ResourceId rid)
    {
        if (auto it = m_resources.find(rid); it != m_resources.end()) {
            return it->second;
        }
        else {
            return {};
        }
    }

    [[nodiscard]] ResourceHandle<Resource> Handle(const ResourceId rid) const
    {
        if (auto it = m_resources.find(rid); it != m_resources.cend()) {
            return it->second;
        }
        else {
            return {};
        }
    }

    bool Contains(const ResourceId rid) const
    {
        return m_resources.find(rid) != m_resources.end();
    }

   private:
    std::unordered_map<ResourceId, ResourceHandle<Resource>> m_resources;
};

}  // namespace SD

#endif  // !SD_RESOURCE_CACHE_HPP
