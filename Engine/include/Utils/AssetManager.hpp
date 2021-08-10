#ifndef SD_ASSET_MANAGER_HPP
#define SD_ASSET_MANAGER_HPP

#include "Utils/AssetLoader.hpp"

namespace sd {

class SD_API AssetManager {
   public:
    AssetManager() = default;

    virtual ~AssetManager() = default;

    AssetManager(const AssetManager &) = delete;
    AssetManager &operator=(const AssetManager &) = delete;

    template <typename LOADER, typename... ARGS>
    void setLoader(ARGS &&...args);

    template <typename ASSET>
    Ref<ASSET> load(const std::string &filePath);

    template <typename ASSET>
    bool hasLoaded(const std::string &name);

    void setRootPath(const std::string &path) { m_rootPath = path; }

    const std::string &getRootPath() const { return m_rootPath; };

   private:
    std::unordered_map<size_t, Scope<AssetLoaderBase>> m_loaders;
    std::string m_rootPath;
};

template <typename LOADER, typename... ARGS>
void AssetManager::setLoader(ARGS &&...args) {
    size_t type = LOADER::getType();
    m_loaders[type] = createScope<LOADER>(*this, std::forward<ARGS>(args)...);
}

template <typename ASSET>
Ref<ASSET> AssetManager::load(const std::string &filePath) {
    size_t type = getLoaderType<ASSET>();
    Ref<void> asset = m_loaders.at(type)->load(filePath);
    return std::static_pointer_cast<ASSET>(asset);
}

template <typename ASSET>
bool AssetManager::hasLoaded(const std::string &name) {
    auto iter = m_loaders.find(getLoaderType<ASSET>());
    if (iter != m_loaders.end()) {
        return iter->second->hasLoaded(name);
    } else {
        return false;
    }
}

}  // namespace sd

#endif /* SD_ASSET_MANAGER_HPP */
