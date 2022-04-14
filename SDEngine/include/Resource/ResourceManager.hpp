#ifndef SD_RESOURCE_MANAGER_HPP
#define SD_RESOURCE_MANAGER_HPP

#include "Utility/Base.hpp"
#include "Resource/Export.hpp"
#include "Resource/Resource.hpp"

#include <functional>

namespace SD {

struct Cache {
    Ref<Resource> data;
    std::string path;

    SERIALIZE(path)
};
using CacheMap = std::unordered_map<ResourceId, Cache>;
using PathMap = std::unordered_map<std::string, ResourceId>;

struct ResourceTypeData {
    std::vector<std::string> extensions;
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

    class ArchiveBase {
       public:
        virtual ~ArchiveBase() = default;
    };

    template <typename T>
    class Serializer : public ArchiveBase {
       public:
        using Signature = std::function<Ref<T>(const std::string& path)>;
        Signature function;
    };
    template <typename T>
    class Deserializer : public ArchiveBase {
       public:
        using Signature =
            std::function<void(const T& obj, const std::string& path)>;
        Signature function;
    };

   public:
    static ResourceManager& Get();
    void Init(const std::filesystem::path& path);
    void ShutDown();

    const std::filesystem::path& GetDirectory() const { return m_directory; }

    template <typename T>
    void Register(const ResourceTypeData& type_data,
                  typename Serializer<T>::Signature&& serialze_func,
                  typename Deserializer<T>::Signature&& deserialze_func)
    {
        TypeId tid = GetTypeId<T>();
        m_resource_types[tid] = type_data;
        if (serialze_func) {
            auto serializer = CreateScope<Serializer<T>>();
            serializer->function = std::move(serialze_func);
            m_serializer[tid] = std::move(serializer);
        }
        if (deserialze_func) {
            auto deserializer = CreateScope<Deserializer<T>>();
            deserializer->function = std::move(deserialze_func);
            m_deserializer[tid] = std::move(deserializer);
        }
    }

    template <typename... Args>
    void FillDataWithPriority()
    {
        (FillData<Args>(), ...);
    }

    template <typename T>
    void FillData()
    {
        TypeId tid = GetTypeId<T>();
        auto& caches = m_caches[tid];
        Serializer<T>* serializer =
            dynamic_cast<Serializer<T>*>(m_serializer.at(tid).get());
        for (auto& [rid, cache] : caches) {
            Ref<T> ptr = serializer->function(
                (m_directory / cache.path).generic_string());
            cache.data = ptr;
            ptr->m_id = rid;
        }
    }

    template <typename T>
    bool SaveResource(const ResourceId rid)
    {
        TypeId tid = GetTypeId<T>();
        if (m_deserializer.count(tid) != 0) {
            auto obj = GetResource<T>(rid);
            auto path = GetPath<T>(rid);
            auto& path_id = m_path_ids[tid];
            path_id[path] = rid;
            m_is_modified = true;
            return true;
        }
        else {
            SD_CORE_ERROR("Unregistered deserializer, saving failed!");
            return false;
        }
    }

    template <typename T>
    Ref<T> LoadResource(const std::string& path)
    {
        static_assert(std::is_base_of<Resource, T>::value,
                      "Resource must be derived from resource.");
        const std::string& full_path = (m_directory / path).generic_string();
        SD_CORE_TRACE("Load Resource:{}", full_path);

        TypeId tid = GetTypeId<T>();
        auto& ids = m_path_ids[tid];
        std::string relative_path = GetRelativePath(full_path);
        ResourceId rid = ids[relative_path];
        if (Exist(tid, rid)) {
            return GetResource<T>(rid);
        }
        else {
            if (m_serializer.count(tid) != 0) {
                m_is_modified = true;
                auto serializer =
                    dynamic_cast<Serializer<T>*>(m_serializer.at(tid).get());
                Ref<T> ptr = serializer->function(full_path);
                auto& caches = m_caches[tid];
                ptr->m_id = rid;
                caches[rid].data = ptr;
                caches[rid].path = relative_path;
                return ptr;
            }
            else {
                SD_CORE_ERROR("Unregistered serializer, loading failed!");
                return nullptr;
            }
        }
    }

    template <typename T>
    Ref<T> GetResource(const ResourceId rid)
    {
        auto& caches = m_caches[GetTypeId<T>()];
        return std::static_pointer_cast<T>(caches[rid].data);
    }

    template <typename T>
    std::string GetPath(const ResourceId rid)
    {
        auto& caches = m_caches[GetTypeId<T>()];
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
        return Exist(GetTypeId<T>(), rid);
    }

    template <typename T>
    bool Empty()
    {
        return Empty(GetTypeId<T>());
    }

    template <typename T>
    const CacheMap& GetCaches()
    {
        return m_caches.at(GetTypeId<T>());
    }

    template <typename T>
    bool MatchExtension(const std::string& ext)
    {
        auto& type_data = m_resource_types.at(GetTypeId<T>());
        auto begin = type_data.extensions.begin();
        auto end = type_data.extensions.end();
        return std::find(begin, end, ext) != end;
    }

   private:
    bool Empty(const TypeId tid) { return m_caches.count(tid) == 0; }

    bool Exist(const TypeId tid, const ResourceId rid) const
    {
        return m_caches.count(tid) != 0 && m_caches.at(tid).count(rid) != 0;
    }

    std::string GetRelativePath(const std::string& path)
    {
        return std::filesystem::relative(path, m_directory).generic_string();
    }

    std::unordered_map<TypeId, CacheMap> m_caches;
    std::unordered_map<TypeId, PathMap> m_path_ids;
    std::unordered_map<TypeId, ResourceTypeData> m_resource_types;
    std::unordered_map<TypeId, Scope<ArchiveBase>> m_serializer;
    std::unordered_map<TypeId, Scope<ArchiveBase>> m_deserializer;
    std::filesystem::path m_directory;
    bool m_is_modified;
};

}  // namespace SD

#endif
