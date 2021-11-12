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
inline size_t getAssetType() {
    return typeid(ASSET).hash_code();
}

class SD_API Asset {
   public:
    Asset();
    Asset(size_t loaderType, const std::string &path);

    size_t getLoaderType() const { return m_loaderType; }
    void setLoaderType(size_t type) { m_loaderType = type; }

    Ref<void> getResource() { return m_resource; }
    const Ref<void> getResource() const { return m_resource; }

    void setResource(const Ref<void> &resource) { m_resource = resource; }

    const std::string &getPath() const { return m_path; }

    template <typename Archive>
    void serialize(Archive &archive) {
        archive(m_loaderType, m_path);
    }

   private:
    Ref<void> m_resource;
    size_t m_loaderType;
    std::string m_path;
};

class SD_API AssetLoaderBase {
   public:
    AssetLoaderBase() = default;
    AssetLoaderBase(const AssetLoaderBase &) = delete;
    AssetLoaderBase &operator=(const AssetLoaderBase &) = delete;
    virtual ~AssetLoaderBase() = default;

    virtual Ref<void> loadAsset(const std::string &path) = 0;
};

class SD_API AssetManager {
   private:
    AssetManager();
    ~AssetManager();

    void clear();

    void cache(const ResourceId &id);

   public:
    void load(const std::filesystem::path &path);
    void save();

    void setDirectory(const std::filesystem::path &path);
    std::filesystem::path getRootPath() const;

    std::filesystem::path getRelativePath(
        const std::filesystem::path &path) const;
    std::filesystem::path getAbsolutePath(
        const std::filesystem::path &path) const;

    bool hasLoaded(const std::string &path) const;

    bool hasCached(const ResourceId &id) const;

    bool valid() const { return std::filesystem::exists(m_directory); }

    static AssetManager &instance() {
        static AssetManager s_instance;
        return s_instance;
    }

    template <typename ASSET>
    void setLoader(AssetLoaderBase *loader) {
        size_t type = getAssetType<ASSET>();
        m_loaders[type] = loader;
    }

    AssetLoaderBase *getLoader(size_t id) { return m_loaders[id]; }

    template <typename ASSET>
    ResourceId loadAsset(const std::string &path) {
        SD_CORE_ASSERT(valid(), "AssetManager's root path is invalid!");
        // generate a random id
        ResourceId id;
        size_t type = getAssetType<ASSET>();
        std::filesystem::path fullPath = getAbsolutePath(path);
        std::string relPath = getRelativePath(fullPath).string();
        // check if loaded in asset
        if (hasLoaded(relPath)) {
            id = m_loaded.at(relPath);
        } else {
            m_loaded.emplace(relPath, id);
            m_resources[id] = Asset(type, relPath);
            cache(id);
        }
        return id;
    }

    template <typename ASSET>
    Ref<ASSET> get(ResourceId id) {
        if (m_resources.count(id) == 0) {
            return nullptr;
        }
        if (!hasCached(id)) {
            cache(id);
        }
        return std::static_pointer_cast<ASSET>(
            m_resources.at(id).getResource());
    }

   private:
    std::unordered_map<std::string, ResourceId> m_loaded;
    std::unordered_map<ResourceId, Asset> m_resources;
    std::unordered_map<size_t, AssetLoaderBase *> m_loaders;
    std::filesystem::path m_directory;
};

};  // namespace SD

#endif
