#ifndef SD_ASSET_HPP
#define SD_ASSET_HPP

#include "Utility/Base.hpp"
#include "Utility/ResourceId.hpp"
#include "Utility/Serialize.hpp"

#include <string>
#include <unordered_map>
#include <typeinfo>
#include <filesystem>

namespace SD {

template <typename ASSET>
inline size_t GetAssetType() {
    return typeid(ASSET).hash_code();
}

class SD_API Asset {
   public:
    Asset();
    Asset(size_t loaderType, const std::string &path);

    size_t GetLoaderType() const { return m_loaderType; }
    void SetLoaderType(size_t type) { m_loaderType = type; }

    Ref<void> GetResource() { return m_resource; }
    const Ref<void> GetResource() const { return m_resource; }

    void SetResource(const Ref<void> &resource) { m_resource = resource; }

    const std::string &GetPath() const { return m_path; }

    template <typename Archive>
    void serialize(Archive &archive) {
        archive(m_loaderType, m_path);
    }

   private:
    Ref<void> m_resource;
    size_t m_loaderType;
    std::string m_path;
};

class AssetManager;

class SD_API AssetLoaderBase {
   public:
    AssetLoaderBase(AssetManager &manager) : m_manager(manager) {}
    AssetLoaderBase(const AssetLoaderBase &) = delete;
    AssetLoaderBase &operator=(const AssetLoaderBase &) = delete;
    virtual ~AssetLoaderBase() = default;

    virtual Ref<void> LoadAsset(const std::filesystem::path &path) = 0;

    AssetManager &Manager() { return m_manager; }

   private:
    AssetManager &m_manager;
};

class SD_API AssetManager {
   private:
    void Clear();

    void Cache(const ResourceId &id);

   public:
    AssetManager(const std::filesystem::path &path);
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

    bool valid() const { return std::filesystem::exists(m_directory); }

    template <typename ASSET>
    void SetLoader(AssetLoaderBase *loader) {
        size_t type = GetAssetType<ASSET>();
        m_loaders[type] = loader;
    }

    AssetLoaderBase *getLoader(size_t id) { return m_loaders[id]; }

    template <typename ASSET>
    ResourceId LoadAsset(const std::filesystem::path &path) {
        SD_CORE_ASSERT(valid(), "AssetManager's root path is invalid!");
        // generate a random id
        ResourceId id;
        size_t type = GetAssetType<ASSET>();
        std::filesystem::path full_path = GetAbsolutePath(path);
        std::string rel_path = GetRelativePath(full_path).generic_string();
        // check if the relative path has id
        if (HasId(rel_path)) {
            id = m_id_map.at(rel_path);
        } else {
            m_id_map.emplace(rel_path, id);
            m_resources[id] = Asset(type, rel_path);
            Cache(id);
        }
        return id;
    }

    template <typename ASSET>
    Ref<ASSET> Get(ResourceId id) {
        if (m_resources.count(id) == 0) {
            return nullptr;
        }
        if (!HasCached(id)) {
            Cache(id);
        }
        return std::static_pointer_cast<ASSET>(
            m_resources.at(id).GetResource());
    }

   private:
    std::unordered_map<std::string, ResourceId> m_id_map;
    std::unordered_map<ResourceId, Asset> m_resources;
    std::unordered_map<size_t, AssetLoaderBase *> m_loaders;
    std::filesystem::path m_directory;
};

};  // namespace SD

#endif
