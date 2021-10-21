#ifndef SD_ASSET_LOADER_HPP
#define SD_ASSET_LOADER_HPP

#include <string>
#include <filesystem>
#include <unordered_map>
#include "Utility/Base.hpp"
#include "Utility/Log.hpp"

namespace sd {

class AssetManager;

template <typename ASSET>
inline size_t getLoaderType() {
    return typeid(ASSET).hash_code();
}

class SD_API AssetLoaderBase {
   public:
    AssetLoaderBase(AssetManager &manager) : m_manager(manager) {}
    virtual ~AssetLoaderBase() = default;

    AssetLoaderBase(const AssetManager &) = delete;
    AssetLoaderBase &operator=(const AssetManager &) = delete;

    virtual Ref<void> load(const std::string &filePath) = 0;

    virtual bool hasLoaded(const std::string &filePath) = 0;

   protected:
    AssetManager &m_manager;
};

template <typename ASSET>
class AssetLoader : public AssetLoaderBase {
   public:
    AssetLoader(AssetManager &manager);

    virtual ~AssetLoader() = default;

   protected:
    friend class AssetManager;

    Ref<void> load(const std::string &filePath) override;

    bool hasLoaded(const std::string &filePath) override;

    static size_t getType() { return getLoaderType<ASSET>(); }

    virtual Ref<ASSET> loadAsset(const std::string &filePath) = 0;

    std::unordered_map<std::string, Ref<ASSET>> m_loaded;
};

class SD_API AssetManager {
   public:
    AssetManager() = default;

    virtual ~AssetManager() = default;

    AssetManager(const AssetManager &) = delete;
    AssetManager &operator=(const AssetManager &) = delete;

    template <typename LOADER, typename... ARGS>
    void setLoader(ARGS &&...args);

    template <typename LOADER>
    LOADER &getLoader();

    template <typename ASSET>
    Ref<ASSET> load(const std::filesystem::path &filePath);

    template <typename ASSET>
    bool hasLoaded(const std::filesystem::path &filePath);

    void setRootPath(const std::filesystem::path &path) {
        m_rootPath =
            path.is_relative() ? std::filesystem::current_path() / path : path;
    }

    std::filesystem::path getRelativePath(const std::filesystem::path &path) {
        return path.lexically_relative(m_rootPath);
    }

    std::filesystem::path getRootPath() const { return m_rootPath; };

    std::filesystem::path getAbsolutePath(
        const std::filesystem::path &filePath) {
        return filePath.is_relative() ? m_rootPath / filePath : filePath;
    }

   private:
    std::unordered_map<size_t, Scope<AssetLoaderBase>> m_loaders;
    std::filesystem::path m_rootPath;
};

class Asset {
   public:
    static AssetManager &manager() {
        static AssetManager s_instance;
        return s_instance;
    }
};

template <typename ASSET>
AssetLoader<ASSET>::AssetLoader(AssetManager &manager)
    : AssetLoaderBase(manager) {
    SD_CORE_TRACE("Initializing AssetLoader<{}>...", typeid(ASSET).name());
}

template <typename ASSET>
Ref<void> AssetLoader<ASSET>::load(const std::string &filePath) {
    Ref<ASSET> asset;

    if (m_loaded.find(filePath) == m_loaded.end()) {
        asset = loadAsset(filePath);
        if (asset != nullptr) {
            m_loaded.emplace(filePath, asset);
        } else {
            SD_CORE_ERROR("Failed to load asset from {}", filePath);
        }
        m_loaded.emplace(filePath, asset);
    } else {
        asset = m_loaded.at(filePath);
    }

    return asset;
}

template <typename ASSET>
bool AssetLoader<ASSET>::hasLoaded(const std::string &filePath) {
    return m_loaded.find(filePath) != m_loaded.end();
}

template <typename LOADER, typename... ARGS>
void AssetManager::setLoader(ARGS &&...args) {
    size_t type = LOADER::getType();
    m_loaders[type] = createScope<LOADER>(*this, std::forward<ARGS>(args)...);
}

template <typename LOADER>
LOADER &AssetManager::getLoader() {
    size_t type = LOADER::getType();
    return *dynamic_cast<LOADER *>(m_loaders[type].get());
}

template <typename ASSET>
Ref<ASSET> AssetManager::load(const std::filesystem::path &filePath) {
    size_t type = getLoaderType<ASSET>();
    Ref<void> asset =
        m_loaders.at(type)->load(getAbsolutePath(filePath).string());
    return std::static_pointer_cast<ASSET>(asset);
}

template <typename ASSET>
bool AssetManager::hasLoaded(const std::filesystem::path &filePath) {
    auto iter = m_loaders.find(getLoaderType<ASSET>());
    if (iter != m_loaders.end()) {
        return iter->second->hasLoaded(getAbsolutePath(filePath).string());
    } else {
        return false;
    }
}

}  // namespace sd

#endif /* SD_ASSET_LOADER_HPP */
