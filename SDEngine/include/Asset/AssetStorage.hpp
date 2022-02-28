#ifndef SD_ASSET_STORAGE_HPP
#define SD_ASSET_STORAGE_HPP

#include "Utility/Base.hpp"
#include "Asset/Asset.hpp"

#include <cstdint>
#include <unordered_map>
#include <functional>
#include <string>

namespace SD {

template <typename T>
inline size_t GetTypeId() {
    return typeid(T).hash_code();
}

using Cache = std::unordered_map<ResourceId, Asset*>;

struct AssetTypeData {
    int m_load_priority{0};
    AssetCreateFunc m_create_func;
    AssetDestroyFunc m_destroy_func;
    std::vector<std::string> m_associated_extensions;
};

class AssetStorage {
   public:
    AssetStorage() = default;
    ~AssetStorage() = default;
    void Init();
    void Shutdown();

    template <typename T>
    bool Exists(ResourceId rid) {
        auto& cache = m_assets[GetTypeId<T>()];
        return cache.find(rid) != cache.end();
    }

    bool Exists(TypeId tid, ResourceId rid) {
        auto& cache = m_assets[tid];
        return cache.find(rid) != cache.end();
    }

    template <typename T>
    T* GetAsset(ResourceId rid) {
        auto& cache = m_assets[GetTypeId<T>()];
        return dynamic_cast<T*>(cache[rid]);
    }

    template <typename T>
    T* LoadAsset(const std::string& path) {
        ResourceId rid(path);
        TypeId tid = GetTypeId<T>();
        T* asset = nullptr;
        if (!Exists(tid, rid)) {
            asset = new T;
            asset->m_directory = m_directory;
            asset->m_path = path;
            asset->m_rid = rid;
            asset->LoadFromFile((m_directory / path).string());
            Add(asset, tid, rid);
        } else {
            asset = GetAsset<T>(rid);
        }
        return asset;
    }

    template <typename T>
    T* CreateAsset(const std::string& path) {
        ResourceId rid(path);
        TypeId tid = GetTypeId<T>();
        T* asset = new T;
        asset->m_directory = m_directory;
        asset->m_path = path;
        asset->m_rid = rid;
        Asset::SaveArchiveToFile((m_directory / path).string(), *asset);
        Add(asset, tid, rid);
        return asset;
    }

    template <typename T>
    void SaveAsset(ResourceId rid) {
        T* obj = GetAsset<T>(rid);
        Asset::SaveArchiveToFile((m_directory / obj->m_path).string(), *obj);
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
        GetTypeData(tid).m_destroy_func(ptr);
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
        GetTypeData(tid).m_destroy_func(ptr);
        cache.erase(rid);
    }

    template <typename T>
    void RegisterAsset(const AssetTypeData& data) {
        m_asset_types[GetTypeId<T>()] = data;
    }

    AssetTypeData& GetTypeData(TypeId tid) { return m_asset_types[tid]; }

    template <typename T>
    const Cache& GetCache() const {
        return m_assets.at(GetTypeId<T>());
    }

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

   private:
    std::unordered_map<TypeId, Cache> m_assets;
    std::unordered_map<TypeId, AssetTypeData> m_asset_types;
    std::filesystem::path m_directory;
};

}  // namespace SD

#endif
