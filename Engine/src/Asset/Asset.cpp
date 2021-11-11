#include "Asset/Asset.hpp"
#include "Asset/ImageLoader.hpp"
#include "Asset/ModelLoader.hpp"
#include "Asset/FontLoader.hpp"

namespace SD {

const std::string assetExt = ".asset";

Asset::Asset() : m_resource(nullptr) {}

Asset::Asset(size_t loaderType, const std::string &path)
    : m_resource(nullptr), m_loaderType(loaderType), m_path(path) {}

AssetManager::AssetManager() {
    setLoader<Image>(new ImageLoader());
    setLoader<Model>(new ModelLoader());
    setLoader<Font>(new FontLoader());
    load("assets");
}

AssetManager::~AssetManager() {
    for (const auto &[id, loader] : m_loaders) {
        delete loader;
    }
    save();
}

void AssetManager::clear() { m_resources.clear(); }

void AssetManager::cache(const ResourceId &id) {
    std::string relPath = m_resources.at(id).getPath();
    std::filesystem::path fullPath = getAbsolutePath(relPath);
    size_t type = m_resources.at(id).getLoaderType();
    Ref<void> resource = m_loaders.at(type)->loadAsset(fullPath);
    SD_CORE_ASSERT(resource, "Invalid asset!");
    m_resources.at(id).setResource(resource);
}

void AssetManager::load(const std::filesystem::path &path) {
    std::string fileName;
    if (std::filesystem::is_directory(path)) {
        fileName = path / assetExt;
    } else {
        if (path.has_extension() && path.extension() == assetExt) {
            fileName = path;
        } else {
            SD_CORE_ASSERT(false, "Invalid asset path!");
        }
    }
    if (std::filesystem::exists(fileName)) {
        clear();
        std::ifstream is(fileName);
        cereal::XMLInputArchive archive(is);
        archive(m_loaded, m_resources);
    }
    setDirectory(std::filesystem::path(fileName).parent_path());
}

void AssetManager::save() {
    std::ofstream os(m_directory / assetExt);
    cereal::XMLOutputArchive archive(os);
    // To have order in serialized data
    std::map<std::string, ResourceId> loaded(m_loaded.begin(), m_loaded.end());
    std::map<ResourceId, Asset> resources(m_resources.begin(), m_resources.end());
    archive(loaded, resources);
}

// FIXME: should use exe path instead of current path
void AssetManager::setDirectory(const std::filesystem::path &path) {
    m_directory =
        path.is_relative()
            ? path
            : std::filesystem::relative(path, std::filesystem::current_path());
}

std::filesystem::path AssetManager::getRelativePath(
    const std::filesystem::path &path) const {
    return std::filesystem::relative(
        path, std::filesystem::current_path() / m_directory);
}

std::filesystem::path AssetManager::getRootPath() const { return m_directory; };

std::filesystem::path AssetManager::getAbsolutePath(
    const std::filesystem::path &path) const {
    return path.is_relative()
               ? std::filesystem::current_path() / m_directory / path
               : path;
}

bool AssetManager::hasLoaded(const std::string &path) const {
    return m_loaded.count(path);
}

bool AssetManager::hasCached(const ResourceId &id) const {
    return m_resources.at(id).getResource() != nullptr;
}

}  // namespace SD
