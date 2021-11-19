#include "Asset/Asset.hpp"
#include "Asset/ImageLoader.hpp"
#include "Asset/ModelLoader.hpp"
#include "Asset/FontLoader.hpp"

namespace SD {

const std::string assetExt = ".asset";

Asset::Asset() : m_resource(nullptr) {}

Asset::Asset(size_t loaderType, const std::string &path)
    : m_resource(nullptr), m_loaderType(loaderType), m_path(path) {}

AssetManager::AssetManager(const std::filesystem::path &path) {
    SetLoader<Image>(new ImageLoader(*this));
    SetLoader<Model>(new ModelLoader(*this));
    SetLoader<Font>(new FontLoader(*this));
    Load(path);
}

AssetManager::~AssetManager() {
    for (const auto &[id, loader] : m_loaders) {
        delete loader;
    }
    Save();
}

void AssetManager::Clear() { m_resources.clear(); }

void AssetManager::Cache(const ResourceId &id) {
    std::string relPath = m_resources.at(id).GetPath();
    std::filesystem::path fullPath = GetAbsolutePath(relPath);
    size_t type = m_resources.at(id).GetLoaderType();
    Ref<void> resource = m_loaders.at(type)->LoadAsset(fullPath);
    SD_CORE_ASSERT(resource, "Invalid asset!");
    m_resources.at(id).SetResource(resource);
}

void AssetManager::Load(const std::filesystem::path &path) {
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
        Clear();
        std::ifstream is(fileName);
        cereal::XMLInputArchive archive(is);
        archive(m_id_map, m_resources);
    }
    SetDirectory(std::filesystem::path(fileName).parent_path());
}

void AssetManager::Save() {
    std::ofstream os(m_directory / assetExt);
    cereal::XMLOutputArchive archive(os);
    // To have order in serialized data
    std::map<std::string, ResourceId> loaded(m_id_map.begin(), m_id_map.end());
    std::map<ResourceId, Asset> resources(m_resources.begin(),
                                          m_resources.end());
    archive(loaded, resources);
}

// FIXME: should use exe path instead of current path
void AssetManager::SetDirectory(const std::filesystem::path &path) {
    m_directory =
        path.is_relative()
            ? path
            : std::filesystem::relative(path, std::filesystem::current_path());
}

std::filesystem::path AssetManager::GetRelativePath(
    const std::filesystem::path &path) const {
    return std::filesystem::relative(
        path, std::filesystem::current_path() / m_directory);
}

std::filesystem::path AssetManager::GetRootPath() const { return m_directory; };

std::filesystem::path AssetManager::GetAbsolutePath(
    const std::filesystem::path &path) const {
    return path.is_relative()
               ? std::filesystem::current_path() / m_directory / path
               : path;
}

bool AssetManager::HasId(const std::string &path) const {
    return m_id_map.count(path);
}

bool AssetManager::HasCached(const ResourceId &id) const {
    return m_resources.at(id).GetResource() != nullptr;
}

}  // namespace SD
