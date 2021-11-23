#include "Asset/Asset.hpp"
#include "Asset/ImageLoader.hpp"
#include "Asset/ModelLoader.hpp"
#include "Asset/FontLoader.hpp"
#include "Asset/ShaderLoader.hpp"

namespace SD {

const std::string ASSET_FILE = ".asset";

Asset::Asset() : m_resource(nullptr) {}

Asset::Asset(size_t loader_type, const std::string &path)
    : m_resource(nullptr), m_loader_type(loader_type), m_path(path) {}

AssetManager::AssetManager(const std::filesystem::path &path) {
    SetLoader<Image>(new ImageLoader(*this));
    SetLoader<Model>(new ModelLoader(*this));
    SetLoader<Font>(new FontLoader(*this));
    SetLoader<Shader>(new ShaderLoader(*this));
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
    std::string rel_path = m_resources.at(id).GetPath();
    std::string full_path = GetAbsolutePath(rel_path).string();
    size_t type = m_resources.at(id).GetLoaderType();
    if (m_loaders.count(type) == 0) {
        throw Exception("Loader not set up correctly!");
    }
    Ref<void> resource = m_loaders.at(type)->LoadAsset(full_path);
    SD_CORE_ASSERT(resource, "Invalid asset!");
    m_resources.at(id).SetResource(resource);
}

void AssetManager::Load(const std::filesystem::path &path) {
    std::filesystem::path filename;
    if (std::filesystem::is_directory(path)) {
        filename = path / ASSET_FILE;
    } else {
        if (path.has_extension() && path.extension() == ASSET_FILE) {
            filename = path;
        } else {
            SD_CORE_ASSERT(false, "Invalid asset path!");
        }
    }
    if (std::filesystem::exists(filename)) {
        Clear();
        std::ifstream is(filename, std::ios::binary);
        cereal::XMLInputArchive archive(is);
        archive(m_id_map, m_resources);
    }
    SetDirectory(std::filesystem::path(filename).parent_path());
}

void AssetManager::Save() {
    std::ofstream os(m_directory / ASSET_FILE, std::ios::binary);
    cereal::XMLOutputArchive archive(os);
    // To have order in serialized data
    std::map<std::string, ResourceId> loaded(m_id_map.begin(), m_id_map.end());
    std::map<ResourceId, Asset> resources(m_resources.begin(),
                                          m_resources.end());
    archive(loaded, resources);
}

void AssetManager::SetDirectory(const std::filesystem::path &path) {
    m_directory =
        path.is_relative() ? std::filesystem::current_path() / path : path;
}

std::filesystem::path AssetManager::GetRelativePath(
    const std::filesystem::path &path) const {
    return path.lexically_relative(m_directory);
}

std::filesystem::path AssetManager::GetRootPath() const { return m_directory; };

std::filesystem::path AssetManager::GetAbsolutePath(
    const std::filesystem::path &path) const {
    auto ret = path.is_relative() ? m_directory / path : path;
    return ret.make_preferred(); 
}

bool AssetManager::HasId(const std::string &path) const {
    return m_id_map.count(path);
}

bool AssetManager::HasCached(const ResourceId &id) const {
    return m_resources.at(id).GetResource() != nullptr;
}

}  // namespace SD
