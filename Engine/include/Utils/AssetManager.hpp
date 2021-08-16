#ifndef SD_ASSET_MANAGER_HPP
#define SD_ASSET_MANAGER_HPP

#include <filesystem>
#include "Common/Export.hpp"
#include "Common/Base.hpp"

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

    std::filesystem::path getRootPath() const { return m_rootPath; };

    std::filesystem::path getAbsolutePath(
        const std::filesystem::path &filePath) {
        return filePath.is_relative() ? m_rootPath / filePath : filePath;
    }

   private:
    std::unordered_map<size_t, Scope<AssetLoaderBase>> m_loaders;
    std::filesystem::path m_rootPath;
};

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

#endif /* SD_ASSET_MANAGER_HPP */
