#ifndef SD_ASSET_HPP
#define SD_ASSET_HPP

#include "Utility/Base.hpp"
#include "Utility/Export.hpp"
#include "Utility/Log.hpp"
#include "Utility/ResourceId.hpp"
#include "Utility/Serialize.hpp"
#include <string>
#include <unordered_map>
#include <typeinfo>
#include <filesystem>

namespace sd {

template <typename ASSET>
inline size_t getLoaderType() {
    return typeid(ASSET).hash_code();
}

class SD_API Asset {
   public:
    Asset() = default;
    Asset(void *resource, size_t loaderType);

    template <typename RESOURCE>
    RESOURCE *getResource() {
        return static_cast<RESOURCE *>(m_resource);
    }

    ResourceId getId() const { return m_id; }

    size_t getLoaderType() const { return m_loaderType; }

    template <typename Archive>
    void serialize(Archive &archive) {
        archive(m_id, m_loaderType);
    }

   private:
    void *m_resource;

    ResourceId m_id;
    size_t m_loaderType;
};

class SD_API AssetLoaderBase {
   public:
    AssetLoaderBase() = default;
    AssetLoaderBase(const AssetLoaderBase &) = delete;
    AssetLoaderBase &operator=(const AssetLoaderBase &) = delete;
    virtual ~AssetLoaderBase() = default;

    virtual Ref<void> loadAsset(const std::string &filePath) = 0;
};

template <typename ASSET>
class SD_API AssetLoader : public AssetLoaderBase {
   protected:
    AssetLoader() = default;

    virtual ~AssetLoader() = default;

   public:
    static size_t getType() { return getLoaderType<ASSET>(); }
};

class SD_API AssetManager {
   private:
    AssetManager();
    ~AssetManager();

    void load(const std::string &filePath);
    void save(const std::string &filePath);
    void reload();

   public:
    static AssetManager &instance() {
        static AssetManager s_instance;
        return s_instance;
    }

    template <typename LOADER, typename... ARGS>
    void setLoader(ARGS &&...args) {
        size_t type = LOADER::getType();
        m_loaders[type] = new LOADER(std::forward<ARGS>(args)...);
    }

    template <typename ASSET>
    Asset loadAsset(const std::string &filePath) {
        size_t type = getLoaderType<ASSET>();
        if (hasLoaded(filePath)) {
            return m_assets[filePath];
        } else {
            std::string fullPath = getAbsolutePath(filePath).string();
            Ref<void> resource =
                dynamic_cast<AssetLoader<ASSET> *>(m_loaders.at(type))
                    ->loadAsset(fullPath);
            Asset asset(resource.get(), type);

            m_memory[asset.getId()] = resource;

            m_assets[fullPath] = asset;
            return asset;
        }
    }

    bool hasLoaded(const std::string &filePath) const {
        return m_assets.count(getAbsolutePath(filePath).string());
    }

    template <typename ASSET>
    Ref<ASSET> get(ResourceId id) {
        return std::static_pointer_cast<ASSET>(m_memory.at(id));
    }

    template <typename ASSET>
    const Ref<ASSET> get(ResourceId id) const {
        return std::static_pointer_cast<ASSET>(m_memory.at(id));
    }

    void setRootPath(const std::filesystem::path &path) {
        m_rootPath =
            path.is_relative() ? std::filesystem::current_path() / path : path;
    }

    std::filesystem::path getRelativePath(
        const std::filesystem::path &path) const {
        return path.lexically_relative(m_rootPath);
    }

    std::filesystem::path getRootPath() const { return m_rootPath; };

    std::filesystem::path getAbsolutePath(
        const std::filesystem::path &filePath) const {
        return filePath.is_relative() ? m_rootPath / filePath : filePath;
    }

   private:
    std::unordered_map<std::string, Asset> m_assets;
    std::unordered_map<ResourceId, Ref<void>> m_memory;
    std::unordered_map<size_t, AssetLoaderBase *> m_loaders;
    std::filesystem::path m_rootPath;
};

};  // namespace sd

#endif
