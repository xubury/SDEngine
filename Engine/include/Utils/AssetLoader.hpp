#ifndef SD_ASSET_LOADER_HPP
#define SD_ASSET_LOADER_HPP

#include <string>
#include <unordered_map>
#include "Common/Base.hpp"
#include "Utils/Log.hpp"

namespace sd {

class AssetManager;

class SD_API AssetLoaderBase {
   public:
    AssetLoaderBase(AssetManager &manager) : m_manager(manager) {}
    virtual ~AssetLoaderBase() = default;

    AssetLoaderBase(const AssetManager &) = delete;
    AssetLoaderBase &operator=(const AssetManager &) = delete;

    virtual Ref<void> load(const std::string &filePath) = 0;

    virtual bool hasLoaded(const std::string &name) = 0;

   protected:
    AssetManager &m_manager;
};

template <typename ASSET>
inline size_t getLoaderType() {
    return typeid(ASSET).hash_code();
}

template <typename ASSET>
class AssetLoader : public AssetLoaderBase {
   public:
    AssetLoader(AssetManager &manager);

    virtual ~AssetLoader() = default;

    Ref<void> load(const std::string &filePath) override;

    bool hasLoaded(const std::string &name) override;

    static size_t getType() { return getLoaderType<ASSET>(); }

   protected:
    virtual Ref<ASSET> loadAsset(const std::string &filePath) = 0;

    std::unordered_map<std::string, Ref<ASSET>> m_loaded;
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
bool AssetLoader<ASSET>::hasLoaded(const std::string &name) {
    return m_loaded.find(name) != m_loaded.end();
}

}  // namespace sd

#endif /* SD_ASSET_LOADER_HPP */
