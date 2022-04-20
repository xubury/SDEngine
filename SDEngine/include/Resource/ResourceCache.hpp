#ifndef SD_RESOURCE_CACHE_HPP
#define SD_RESOURCE_CACHE_HPP

#include "Utility/ResourceId.hpp"
#include "Resource/ResourceHandle.hpp"
#include <unordered_map>
#include <cstdint>

namespace SD {

template <typename Resource, typename Loader>
class ResourceCache {
   public:
    size_t Size() const { return m_resources.size(); }

    bool Empty() const { return m_resources.empty(); }

    template <typename... Args>
    ResourceHandle<Resource> Load(const ResourceId rid, Args &&...args)
    {
        if (auto it = m_resources.find(rid); it == m_resources.cend()) {
            if (auto handle = Temp(std::forward<Args>(args)...); handle) {
                handle.SetIdentifier(rid);
                return m_resources[rid] = std::move(handle);
            }
        }
        else {
            return it->second;
        }

        return {};
    }

    template <typename... Args>
    [[nodiscard]] ResourceHandle<Resource> Temp(Args &&...args)
    {
        return m_loader(std::forward<Args>(args)...);
    }

    void Discard(const ResourceId rid)
    {
        if (auto it = m_resources.find(rid); it != m_resources.end()) {
            m_resources.erase(it);
        }
    }

    template <typename... Args>
    [[nodiscard]] ResourceHandle<Resource> Reload(const ResourceId rid,
                                                  Args &&...args)
    {
        return (Discard(rid), Load(rid, std::forward<Args>(args)...));
    }

    [[nodiscard]] ResourceHandle<Resource> Get(const ResourceId rid)
    {
        if (auto it = m_resources.find(rid); it != m_resources.end()) {
            return it->second;
        }
        else {
            return {};
        }
    }

    [[nodiscard]] ResourceHandle<Resource> Get(const ResourceId rid) const
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
    Loader m_loader;
    std::unordered_map<ResourceId, ResourceHandle<Resource>> m_resources;
};

}  // namespace SD

#endif  // !SD_RESOURCE_CACHE_HPP
