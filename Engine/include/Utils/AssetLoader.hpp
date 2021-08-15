#ifndef SD_ASSET_LOADER_HPP
#define SD_ASSET_LOADER_HPP

#include <string>
#include <filesystem>
#include <unordered_map>
#include "Common/Base.hpp"
#include "Common/Log.hpp"
#include "AssetManager.hpp"

namespace sd {

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

}  // namespace sd

#endif /* SD_ASSET_LOADER_HPP */
