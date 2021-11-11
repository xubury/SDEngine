#include "Asset/Asset.hpp"
#include "Asset/ImageLoader.hpp"
#include "Asset/ModelLoader.hpp"
#include "Asset/FontLoader.hpp"

namespace sd {

Asset::Asset(size_t loaderType) : m_loaderType(loaderType) {}

AssetManager::AssetManager() {
    setRootPath("assets/");
    setLoader<ImageLoader>();
    setLoader<ModelLoader>();
    setLoader<FontLoader>();

    load(m_rootPath / "test.asset");
}

AssetManager::~AssetManager() {
    save(m_rootPath / "test.asset");

    for (const auto &[id, loader] : m_loaders) {
        delete loader;
    }
}

void AssetManager::load(const std::string &filePath) {
    m_assets.clear();

    std::ifstream is(filePath, std::ios::binary);
    cereal::XMLInputArchive archive(is);
    archive(m_assets);

    reload();
}
void AssetManager::save(const std::string &filePath) {
    std::ofstream os(filePath);
    cereal::XMLOutputArchive archive(os);
    archive(m_assets);
}

void AssetManager::reload() {
    m_memory.clear();
    for (const auto &[path, asset] : m_assets) {
        m_memory[(asset.getId())] = m_loaders.at(asset.getLoaderType())
                                        ->loadAsset(getAbsolutePath(path));
    }
}

}  // namespace sd
