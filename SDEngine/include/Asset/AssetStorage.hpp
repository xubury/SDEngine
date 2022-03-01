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
    int load_priority{0};
    AssetCreateFunc create_func;
    AssetDestroyFunc destroy_func;
};

class SD_ASSET_API AssetStorage {
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
        std::string full_path = (m_directory / path).generic_string();

        ResourceId rid(full_path);
        TypeId tid = GetTypeId<T>();
        T* asset = nullptr;
        if (!Exists(tid, rid)) {
            asset = new T;
            asset->m_path = full_path;
            asset->m_rid = rid;
            Asset::LoadArchiveFromFile(asset->m_path, asset);
            asset->Init();
            Add(asset, tid, rid);
        } else {
            asset = GetAsset<T>(rid);
        }
        return asset;
    }

    template <typename T>
    T* CreateAsset(const std::string& path) {
        std::string full_path = (m_directory / path).generic_string();
        ResourceId rid(full_path);
        TypeId tid = GetTypeId<T>();
        T* asset = nullptr;
        if (Exists(tid, rid)) {
            SD_CORE_WARN(
                "Trying to create an asset that already exists, returning the "
                "existing one.");
            asset = GetAsset<T>(rid);
        } else {
            asset = new T;
            asset->m_path = full_path;
            asset->m_rid = rid;
            Asset::SaveArchiveToFile(asset->m_path, asset);
            Add(asset, tid, rid);
        }
        return asset;
    }

    template <typename T>
    void SaveAsset(ResourceId rid) {
        T* asset = GetAsset<T>(rid);
        Asset::SaveArchiveToFile(asset->m_path, asset);
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
