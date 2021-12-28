#include "Asset/Asset.hpp"

namespace SD {

const std::string ASSET_FILE = ".asset";

Asset::Asset() : m_resource(nullptr) {}

Asset::Asset(size_t loader_type, const std::string &path)
    : m_resource(nullptr), m_loader_type(loader_type), m_path(path) {}

AssetManager::AssetManager() { SD_CORE_TRACE("Initalizing AssetManager"); }

AssetManager::~AssetManager() {
    SD_CORE_TRACE("Deleting AssetManager");
    Clear();
    for (const auto &[id, loader] : m_loaders) {
        delete loader;
    }
}

void AssetManager::Clear() {
    std::lock_guard<std::shared_mutex> lock(m_mutex);
    m_id_map.clear();
    m_resources.clear();
}

Ref<void> AssetManager::Cache(size_t type, const std::string &rel_path) {
    std::string full_path = GetAbsolutePath(rel_path).string();
    Ref<void> resource = m_loaders.at(type)->LoadAsset(full_path);
    if (!resource) {
        throw Exception("Invalid asset!");
    } else {
    }
    return resource;
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
    Validate();
}

void AssetManager::Save() {
    std::shared_lock<std::shared_mutex> shared_lock(m_mutex);
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
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    return m_id_map.count(path);
}

void AssetManager::Validate() {
    // remove asset that no longer exists.
    std::lock_guard<std::shared_mutex> lock(m_mutex);
    auto iter = m_id_map.begin();
    while (iter != m_id_map.end()) {
        size_t sep_pos = iter->first.find_first_of('@');
        if (sep_pos == std::string::npos) {
            iter = m_id_map.erase(iter);
            SD_CORE_WARN("Invalid id map");
            continue;
        }
        std::string path = iter->first.substr(sep_pos + 1);
        if (!std::filesystem::exists(GetAbsolutePath(path))) {
            iter = m_id_map.erase(iter);
        } else {
            ++iter;
        }
    }
}

}  // namespace SD
