#ifndef SD_RESOURCE_MANAGER_HPP
#define SD_RESOURCE_MANAGER_HPP

#include "Utility/ResourceId.hpp"
#include "Resource/Export.hpp"

#include <functional>

namespace SD {

using ResourceCache = std::unordered_map<ResourceId, Ref<void>>;

struct ResourceTypeData {
};

using TypeId = size_t;

class SD_RESOURCE_API ResourceManager {
   private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    ResourceManager(const ResourceManager&) = default;
    ResourceManager& operator=(const ResourceManager&) = default;

    template <typename T>
    static TypeId GetTypeId()
    {
        return typeid(T).hash_code();
    }

    struct Archive {
        template <typename T>
        struct Serializer {
            using FuncType = std::function<Ref<T>(const std::string& path)>;
            static FuncType function;
        };
        template <typename T>
        struct Deserializer {
            using FuncType =
                std::function<void(const T& obj, const std::string& path)>;
            static FuncType function;
        };
    };

   public:
    static ResourceManager& Get();

    template <typename T>
    void Register(typename Archive::Serializer<T>::FuncType serializer,
                  typename Archive::Deserializer<T>::FuncType deserializer)
    {
        Archive::Serializer<T>::function = serializer;
        Archive::Deserializer<T>::function = deserializer;
    }

    template <typename T>
    void SaveResource(const ResourceId rid, const std::string& path)
    {
        auto obj = GetResource<T>(rid);
        Archive::Deserializer<T>::function(*obj, path);
    }

    template <typename T>
    Ref<T> LoadResource(const std::string& path)
    {
        SD_CORE_TRACE("Load Resource:{}", path);
        ResourceId rid(path);
        if (Exist<T>(rid)) {
            return GetResource<T>(rid);
        }
        else {
            Ref<void> ptr = Archive::Serializer<T>::function(path);
            auto& cache = GetCache<T>();
            cache[rid] = ptr;
            return std::static_pointer_cast<T>(ptr);
        }
    }

    template <typename T>
    Ref<T> GetResource(const ResourceId rid)
    {
        auto& cache = GetCache<T>();
        return std::static_pointer_cast<T>(cache[rid]);
    }

    template <typename T>
    ResourceId CreateResource(const Ref<T>& obj)
    {
        ResourceId rid;
        auto& cache = GetCache<T>();
        cache[rid] = obj;
        return rid;
    }

    template <typename T>
    void Unload(const ResourceId rid)
    {
        const TypeId tid = GetTypeId<T>();
        auto& cache = m_resources[tid];
        cache.erase(rid);
    }

    template <typename T>
    bool Exist(const std::string& path) const
    {
        return Exist(GetTypeId<T>(), ResourceId(path));
    }

    template <typename T>
    bool Exist(const ResourceId rid) const
    {
        TypeId tid = GetTypeId<T>();
        return m_resources.count(tid) != 0 &&
               m_resources.at(tid).count(rid) != 0;
    }

    template <typename T>
    bool Empty()
    {
        return m_resources.count(GetTypeId<T>()) == 0;
    }

    template <typename T>
    const ResourceTypeData& GetTypeData() const
    {
        return m_resource_types.at(GetTypeId<T>());
    }

    template <typename T>
    ResourceCache& GetCache()
    {
        return m_resources[GetTypeId<T>()];
    }

   private:
    std::unordered_map<TypeId, ResourceCache> m_resources;
    std::unordered_map<TypeId, ResourceTypeData> m_resource_types;
};

template <typename T>
typename ResourceManager::Archive::Serializer<T>::FuncType
    ResourceManager::Archive::Serializer<T>::function;

template <typename T>
typename ResourceManager::Archive::Deserializer<T>::FuncType
    ResourceManager::Archive::Deserializer<T>::function;

}  // namespace SD

#endif
