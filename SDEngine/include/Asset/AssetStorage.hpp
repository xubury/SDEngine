#ifndef SD_ASSET_STORAGE_HPP
#define SD_ASSET_STORAGE_HPP

#include "Utility/Base.hpp"
#include "Asset/Asset.hpp"

#include <cstdint>
#include <unordered_map>
#include <functional>

namespace SD {

using Cache = std::unordered_map<ResourceId, Asset*>;

struct AssetTypeData {
    int load_priority{0};
    AssetCreateFunc create_func;
    AssetDestroyFunc destroy_func;
    std::string file_extension;
};

class SD_ASSET_API AssetStorage {
   public:
    static AssetStorage& Get();
    static void Init();
    static void Shutdown();

    AssetStorage(const AssetStorage&) = delete;
    AssetStorage& operator=(const AssetStorage&) = delete;

    template <typename T>
    bool Exists(ResourceId rid) const
    {
        TypeId tid = GetTypeId<T>();
        if (!Empty(tid)) {
            auto& cache = GetCache(tid);
            return cache.count(rid) != 0;
        }
        return false;
    }

    bool Exists(TypeId tid, ResourceId rid) const
    {
        if (!Empty(tid)) {
            auto& cache = GetCache(tid);
            return cache.count(rid) != 0;
        }
        return false;
    }

    template <typename T>
    T* GetAsset(ResourceId rid) const
    {
        TypeId tid = GetTypeId<T>();
        return dynamic_cast<T*>(GetAsset(tid, rid));
    }

    Asset* GetAsset(TypeId tid, ResourceId rid) const
    {
        return GetCache(tid).at(rid);
    }

    Asset* LoadAsset(const std::string& path)
    {
        std::ifstream is(path, std::ios::binary);
        std::string id(ASSET_IDENTIFIER.size(), ' ');
        is.read(id.data(), id.size());
        if (id != ASSET_IDENTIFIER) {
            throw Exception("Invalid asset file!");
        }
        cereal::PortableBinaryInputArchive iarchive(is);
        return DeserializeAsset(iarchive);
    }

    Asset* DeserializeAsset(cereal::PortableBinaryInputArchive& iarchive)
    {
        TypeId tid;
        ResourceId rid;
        std::string name;
        iarchive(tid, rid, name);

        Asset* obj = nullptr;
        if (Exists(tid, rid)) {
            obj = GetCache(tid).at(rid);
        }
        else {
            obj = m_asset_types.at(tid).create_func();
            obj->m_tid = tid;
            obj->m_rid = rid;
            obj->m_name = name;
            obj->Deserialize(iarchive);
            Add(obj, obj->m_tid, obj->m_rid);
        }
        return obj;
    }

    template <typename T>
    T* CreateAsset(const std::string& name)
    {
        ResourceId rid;
        TypeId tid = GetTypeId<T>();
        Asset* asset = m_asset_types.at(tid).create_func();
        asset->m_tid = tid;
        asset->m_rid = rid;
        asset->m_name = name;
        Add(asset, tid, rid);
        return dynamic_cast<T*>(asset);
    }

    void SaveAsset(Asset* obj, const std::string& path) const
    {
        std::string full_path = ResolvePath(obj->m_tid, path);
        if (std::filesystem::exists(full_path)) {
            std::filesystem::remove(full_path);
        }
        std::ofstream os(full_path, std::ios::binary);
        os << ASSET_IDENTIFIER;
        cereal::PortableBinaryOutputArchive oarchive(os);
        SerializeAsset(obj, oarchive);
    }

    void SerializeAsset(Asset* obj,
                        cereal::PortableBinaryOutputArchive& oarchive) const
    {
        oarchive(obj->m_tid, obj->m_rid, obj->m_name);
        obj->Serialize(oarchive);
    }

    template <typename T>
    void SaveAsset(ResourceId rid, const std::string& path) const
    {
        auto& cache = GetCache<T>();
        SaveAsset(cache.at(rid), path);
    }

    void Add(Asset* data, TypeId tid, ResourceId rid)
    {
        auto& cache = m_assets[tid];
        cache[rid] = data;
    }

    template <typename T>
    void Add(T* data, ResourceId rid)
    {
        Add(data, GetTypeId<T>(), rid);
    }

    template <typename T>
    void Unload(const ResourceId rid)
    {
        TypeId tid = GetTypeId<T>();
        auto& cache = m_assets[tid];
        if (!Exists<T>(rid)) {
            SD_CORE_WARN(
                "The asset you are trying to unload does not exists:{}", rid);
            return;
        }
        auto* ptr = cache[rid];
        GetTypeData(tid).destroy_func(ptr);
        cache.erase(rid);
    }

    void Unload(TypeId tid, ResourceId rid)
    {
        auto& cache = m_assets[tid];
        if (!Exists(tid, rid)) {
            SD_CORE_WARN(
                "The asset you are trying to unload does not exists:{}", rid);
            return;
        }
        auto* ptr = cache[rid];
        GetTypeData(tid).destroy_func(ptr);
        cache.erase(rid);
    }

    template <typename T>
    void RegisterAsset(const AssetTypeData& data)
    {
        TypeId tid = GetTypeId<T>();
        m_asset_types[tid] = data;
    }

    AssetTypeData& GetTypeData(TypeId tid) { return m_asset_types[tid]; }

    const AssetTypeData& GetTypeData(TypeId tid) const
    {
        return m_asset_types.at(tid);
    }

    template <typename T>
    const Cache& GetCache() const
    {
        return m_assets.at(GetTypeId<T>());
    }

    const Cache& GetCache(TypeId tid) const { return m_assets.at(tid); }

    template <typename T>
    bool Empty() const
    {
        return m_assets.count(GetTypeId<T>()) == 0;
    }

    bool Empty(TypeId tid) const { return m_assets.count(tid) == 0; }

    template <typename T>
    bool IsRegistered()
    {
        return m_asset_types.find(GetTypeId<T>()) != m_asset_types.end();
    }

    bool IsRegistered(TypeId tid)
    {
        return m_asset_types.find(tid) != m_asset_types.end();
    }

    void SetDirectory(const std::filesystem::path& directory)
    {
        m_directory = directory.is_relative()
                          ? std::filesystem::current_path() / directory
                          : directory;
    }

    const std::filesystem::path& GetDirectory() const { return m_directory; }

    std::string GetRelativePath(const std::string& path) const
    {
        return std::filesystem::relative(path, m_directory).generic_string();
    }

    std::string GetAbsolutePath(const std::string& path) const
    {
        return (m_directory / path).generic_string();
    }

    void ScanDirectory(const std::filesystem::path& dir);

    const static std::string ASSET_IDENTIFIER;

   private:
    void ScanDirectory(const std::filesystem::path& dir,
                       const std::string& ext);

    std::string ResolvePath(TypeId tid, const std::string& path) const
    {
        std::filesystem::path full_path = m_directory / path;
        full_path += GetTypeData(tid).file_extension;
        return full_path.generic_string();
    }

    AssetStorage() = default;
    ~AssetStorage();

    std::unordered_map<TypeId, Cache> m_assets;
    std::unordered_map<TypeId, AssetTypeData> m_asset_types;
    std::filesystem::path m_directory;
};

}  // namespace SD

#endif
