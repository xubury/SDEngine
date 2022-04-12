#ifndef SD_RESOURCE_MANAGER_HPP
#define SD_RESOURCE_MANAGER_HPP

#include "Utility/Base.hpp"
#include "Resource/Export.hpp"
#include "Resource/ResourceId.hpp"

#include <functional>

namespace SD {

struct Cache {
    Ref<void> data;
    std::string path;

    SERIALIZE(path)
};
using CacheMap = std::unordered_map<ResourceId, Cache>;
using PathMap = std::unordered_map<std::string, ResourceId>;

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

    template <typename T>
    struct Serializer {
        using Signature = std::function<Ref<T>(const std::string& path)>;
        static Signature function;
    };
    template <typename T>
    struct Deserializer {
        using Signature =
            std::function<void(const T& obj, const std::string& path)>;
        static Signature function;
    };

   public:
    static ResourceManager& Get();
    void Init(const std::filesystem::path& path);
    void ShutDown();

    const std::filesystem::path& GetDirectory() const { return m_directory; }

    template <typename T>
    void Register(typename Serializer<T>::Signature&& serializer,
                  typename Deserializer<T>::Signature&& deserializer)
    {
        Serializer<T>::function = std::move(serializer);
        Deserializer<T>::function = std::move(deserializer);
    }

    template <typename... Args>
    void FillDataWithPriority()
    {
        (FillData<Args>(), ...);
    }

    template <typename T>
    void FillData()
    {
        auto& caches = GetCaches<T>();
        for (auto& [rid, cache] : caches) {
            Ref<T> ptr = Serializer<T>::function(
                (m_directory / cache.path).generic_string());
            cache.data = ptr;
            ptr->m_id = rid;
        }
    }

    template <typename T>
    void SaveResource(const ResourceId rid)
    {
        auto obj = GetResource<T>(rid);
        auto path = GetPath<T>(rid);
        auto& path_id = m_path_ids[GetTypeId<T>()];
        path_id[path] = rid;
        Deserializer<T>::function(*obj, path);
        m_is_modified = true;
    }

    template <typename T>
    Ref<T> LoadResource(const std::string& path)
    {
        SD_CORE_TRACE("Load Resource:{}", m_directory / path);
        auto& ids = m_path_ids[GetTypeId<T>()];
        std::string relative_path = GetRelativePath(path);
        ResourceId rid = ids[relative_path];
        if (Exist<T>(rid)) {
            return GetResource<T>(rid);
        }
        else {
            m_is_modified = true;
            Ref<T> ptr = Serializer<T>::function(path);
            auto& caches = GetCaches<T>();
            ptr->m_id = rid;
            caches[rid].data = ptr;
            caches[rid].path = relative_path;
            return ptr;
        }
    }

    template <typename T>
    Ref<T> GetResource(const ResourceId rid)
    {
        auto& caches = GetCaches<T>();
        return std::static_pointer_cast<T>(caches[rid].data);
    }

    template <typename T>
    std::string GetPath(const ResourceId rid)
    {
        auto& caches = GetCaches<T>();
        return caches[rid].path;
    }

    template <typename T>
    void Unload(const ResourceId rid)
    {
        const TypeId tid = GetTypeId<T>();
        auto& cache = m_caches[tid];
        cache.erase(rid);
        m_is_modified = true;
    }

    template <typename T>
    bool Exist(const ResourceId rid) const
    {
        TypeId tid = GetTypeId<T>();
        return m_caches.count(tid) != 0 && m_caches.at(tid).count(rid) != 0;
    }

    template <typename T>
    bool Empty()
    {
        return m_caches.count(GetTypeId<T>()) == 0;
    }

    template <typename T>
    const ResourceTypeData& GetTypeData() const
    {
        return m_resource_types.at(GetTypeId<T>());
    }

    template <typename T>
    CacheMap& GetCaches()
    {
        return m_caches[GetTypeId<T>()];
    }

   private:
    std::string GetRelativePath(const std::string& path)
    {
        return std::filesystem::relative(path, m_directory).generic_string();
    }
    std::unordered_map<TypeId, CacheMap> m_caches;
    std::unordered_map<TypeId, PathMap> m_path_ids;
    std::unordered_map<TypeId, ResourceTypeData> m_resource_types;
    std::filesystem::path m_directory;
    bool m_is_modified;
};

template <typename T>
typename ResourceManager::Serializer<T>::Signature
    ResourceManager::Serializer<T>::function;

template <typename T>
typename ResourceManager::Deserializer<T>::Signature
    ResourceManager::Deserializer<T>::function;

}  // namespace SD

#endif
