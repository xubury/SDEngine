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
};

class SD_ASSET_API AssetStorage {
   public:
    static AssetStorage& Get();
    static void Init();
    static void Shutdown();

    AssetStorage(const AssetStorage&) = delete;
    AssetStorage& operator=(const AssetStorage&) = delete;

    template <typename T>
    bool Exists(ResourceId rid) const {
        TypeId tid = GetTypeId<T>();
        if (!Empty(tid)) {
            auto& cache = GetCache(tid);
            return cache.count(rid) != 0;
        }
        return false;
    }

    bool Exists(TypeId tid, ResourceId rid) const {
        if (!Empty(tid)) {
            auto& cache = GetCache(tid);
            return cache.count(rid) != 0;
        }
        return false;
    }

    template <typename T>
    T* GetAsset(ResourceId rid) const {
        TypeId tid = GetTypeId<T>();
        return dynamic_cast<T*>(GetAsset(tid, rid));
    }

    Asset* GetAsset(TypeId tid, ResourceId rid) const {
        return GetCache(tid).at(rid);
    }

    Asset* LoadAsset(const std::string& path) {
        std::string full_path = ResolvePath(path);
        SD_CORE_TRACE("load asset:{}", full_path);
        std::ifstream is(full_path, std::ios::binary);
        std::string id(ASSET_IDENTIFIER.size(), ' ');
        is.read(id.data(), id.size());
        SD_CORE_TRACE("file id:{}", ASSET_IDENTIFIER);
        if (id != ASSET_IDENTIFIER) {
            throw Exception("Invalid asset file!");
        }
        cereal::PortableBinaryInputArchive iarchive(is);
        TypeId tid;
        ResourceId rid;
        iarchive(tid, rid);

        Asset* obj = nullptr;
        if (Exists(tid, rid)) {
            obj = GetCache(tid).at(rid);
        } else {
            obj = m_asset_types.at(tid).create_func();
            obj->m_tid = tid;
            obj->m_rid = rid;
            obj->m_path = full_path;
            obj->Deserialize(iarchive);
            Add(obj, obj->m_tid, obj->m_rid);
        }
        return obj;
    }

    template <typename T>
    T* CreateAsset(const std::string& path) {
        std::string full_path = ResolvePath(path);
        ResourceId rid;
        TypeId tid = GetTypeId<T>();
        Asset* asset = m_asset_types.at(tid).create_func();
        asset->m_tid = tid;
        asset->m_rid = rid;
        asset->m_path = full_path;
        Add(asset, tid, rid);
        SaveAsset(asset);
        return dynamic_cast<T*>(asset);
    }

    void SaveAsset(Asset* obj) const {
        if (std::filesystem::exists(obj->m_path)) {
            std::filesystem::remove(obj->m_path);
        }
        std::ofstream os(obj->m_path, std::ios::binary);
        os << ASSET_IDENTIFIER;
        cereal::PortableBinaryOutputArchive oarchive(os);
        oarchive(obj->m_tid, obj->m_rid);
        obj->Serialize(oarchive);
    }

    template <typename T>
    void SaveAsset(ResourceId rid) const {
        auto& cache = GetCache<T>();
        SaveAsset(cache.at(rid));
    }

    void Add(Asset* data, TypeId tid, ResourceId rid) {
        auto& cache = m_assets[tid];
        cache[rid] = data;
    }

    template <typename T>
    void Add(T* data, ResourceId rid) {
        Add(data, GetTypeId<T>(), rid);
    }

    template <typename T>
    void Unload(const ResourceId rid) {
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

    void Unload(TypeId tid, ResourceId rid) {
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
    void RegisterAsset(const AssetTypeData& data) {
        TypeId tid = GetTypeId<T>();
        m_asset_types[tid] = data;
    }

    AssetTypeData& GetTypeData(TypeId tid) { return m_asset_types[tid]; }

    template <typename T>
    const Cache& GetCache() const {
        return m_assets.at(GetTypeId<T>());
    }

    const Cache& GetCache(TypeId tid) const { return m_assets.at(tid); }

    template <typename T>
    bool Empty() const {
        return m_assets.count(GetTypeId<T>()) == 0;
    }

    bool Empty(TypeId tid) const { return m_assets.count(tid) == 0; }

    template <typename T>
    bool IsRegistered() {
        return m_asset_types.find(GetTypeId<T>()) != m_asset_types.end();
    }

    bool IsRegistered(TypeId tid) {
        return m_asset_types.find(tid) != m_asset_types.end();
    }

    void SetDirectory(const std::filesystem::path& directory) {
        m_directory = directory.is_relative()
                          ? std::filesystem::current_path() / directory
                          : directory;
    }

    const std::filesystem::path& GetDirectory() const { return m_directory; }

    std::string GetRelativePath(const std::string& path) const {
        return std::filesystem::relative(path, m_directory).generic_string();
    }

    std::string GetAbsolutePath(const std::string& path) const {
        return (m_directory / path).generic_string();
    }

    void ScanDirectory(const std::filesystem::path& dir);

    const static std::string ASSET_IDENTIFIER;
    const static std::string ASSET_POSFIX;

   private:
    std::string ResolvePath(const std::string& path) {
        std::filesystem::path full_path = m_directory / path;
        if (!full_path.has_extension()) {
            full_path += ASSET_POSFIX;
        }
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
