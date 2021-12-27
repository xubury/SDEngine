#ifndef SD_ASSET_HPP
#define SD_ASSET_HPP

#include "Utility/Base.hpp"
#include "Utility/ResourceId.hpp"
#include "Utility/Serialize.hpp"

#include "Asset/Export.hpp"

#include <string>
#include <unordered_map>
#include <typeinfo>
#include <filesystem>
#include <shared_mutex>

namespace SD {

template <typename ASSET>
inline size_t GetAssetType() {
    return typeid(ASSET).hash_code();
}

class SD_ASSET_API Asset {
   public:
    Asset();
    Asset(size_t loaderType, const std::string &path);

    size_t GetLoaderType() const { return m_loader_type; }
    void SetLoaderType(size_t type) { m_loader_type = type; }

    Ref<void> GetResource() { return m_resource; }
    const Ref<void> GetResource() const { return m_resource; }

    void SetResource(const Ref<void> &resource) { m_resource = resource; }

    const std::string &GetPath() const { return m_path; }

    template <typename Archive>
    void serialize(Archive &archive) {
        archive(m_loader_type, m_path);
    }

   private:
    Ref<void> m_resource;
    size_t m_loader_type;
    std::string m_path;
};

class AssetManager;

class SD_ASSET_API AssetLoader {
   public:
    AssetLoader(AssetManager &manager) : m_manager(manager) {}
    AssetLoader(const AssetLoader &) = delete;
    AssetLoader &operator=(const AssetLoader &) = delete;
    virtual ~AssetLoader() = default;

    virtual Ref<void> LoadAsset(const std::string &path) = 0;

    AssetManager &Manager() { return m_manager; }

   private:
    AssetManager &m_manager;
};

class SD_ASSET_API AssetManager {
   private:
    void Clear();

    void Cache(const ResourceId &id, AssetLoader *loader,
               const std::string &rel_path);

   public:
    AssetManager();
    ~AssetManager();

    void Load(const std::filesystem::path &path);
    void Save();

    void SetDirectory(const std::filesystem::path &path);
    std::filesystem::path GetRootPath() const;

    std::filesystem::path GetRelativePath(
        const std::filesystem::path &path) const;
    std::filesystem::path GetAbsolutePath(
        const std::filesystem::path &path) const;

    bool HasId(const std::string &path) const;

    bool HasCached(const ResourceId &id) const;

    bool Valid() const { return std::filesystem::exists(m_directory); }

    template <typename ASSET>
    void SetLoader(AssetLoader *loader) {
        size_t type = GetAssetType<ASSET>();
        m_loaders[type] = loader;
    }

    template <typename ASSET>
    ResourceId LoadAsset(const std::filesystem::path &path) {
        SD_CORE_ASSERT(Valid(), "AssetManager's root path is invalid!");
        std::filesystem::path full_path = GetAbsolutePath(path);
        std::string rel_path = GetRelativePath(full_path).generic_string();
        // check if the relative path has id
        if (HasId(rel_path)) {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            return m_id_map.at(rel_path);
        } else {
            // generate a random id
            ResourceId id;
            size_t type = GetAssetType<ASSET>();
            Cache(id, GetLoader<ASSET>(), rel_path);
            {
                std::lock_guard<std::shared_mutex> lock(m_mutex);
                m_id_map.emplace(rel_path, id);
                m_resources[id] = Asset(type, rel_path);
            }
            return id;
        }
    }

    bool Has(ResourceId id) {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return m_resources.count(id) == 0;
    }

    template <typename ASSET>
    Ref<ASSET> Get(ResourceId id) {
        if (Has(id)) {
            return nullptr;
        }
        if (!HasCached(id)) {
            Cache(id, GetLoader<ASSET>(), GetAssetPath(id));
        }
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return std::static_pointer_cast<ASSET>(
            m_resources.at(id).GetResource());
    }

    // Return relative path
    std::string GetAssetPath(ResourceId id) {
        if (Has(id)) {
            return "";
        }
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return m_resources.at(id).GetPath();
    }

    template <typename ASSET>
    Ref<ASSET> LoadAndGet(const std::filesystem::path &path) {
        auto id = LoadAsset<ASSET>(path);
        return Get<ASSET>(id);
    }

    template <typename ASSET>
    AssetLoader *GetLoader() {
        size_t type = GetAssetType<ASSET>();
        SD_CORE_ASSERT(m_loaders.count(type), "Loader not set up correctly!");
        return m_loaders.at(type);
    }

    void Validate();

   private:
    std::unordered_map<std::string, ResourceId> m_id_map;
    std::unordered_map<ResourceId, Asset> m_resources;
    std::unordered_map<size_t, AssetLoader *> m_loaders;
    std::filesystem::path m_directory;
    mutable std::shared_mutex m_mutex;
};

};  // namespace SD

#endif
