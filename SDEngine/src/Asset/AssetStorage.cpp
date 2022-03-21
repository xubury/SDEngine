#include "Asset/AssetStorage.hpp"

namespace SD {

const std::string AssetStorage::ASSET_IDENTIFIER = "SD_ASSET";

static AssetStorage *s_storage = nullptr;

AssetStorage &AssetStorage::Get() { return *s_storage; }

void AssetStorage::Init() { s_storage = new AssetStorage; }

void AssetStorage::Shutdown()
{
    delete s_storage;
    s_storage = nullptr;
}

AssetStorage::~AssetStorage()
{
    for (auto &[res_type, cache] : m_assets) {
        for (auto &[sid, ptr] : cache) {
            GetTypeData(res_type).destroy_func(ptr);
        }
        cache.clear();
    }
    m_assets.clear();
}

void AssetStorage::ScanDirectory(const std::filesystem::path &dir,
                                 const std::string &ext)
{
    for (auto &entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_directory()) {
            ScanDirectory(entry, ext);
        }
        else if (entry.is_regular_file() && entry.path().extension() == ext) {
            LoadAsset(entry.path().generic_string());
        }
    }
}

void AssetStorage::ScanDirectory(const std::filesystem::path &dir)
{
    // sort loading order
    using ExtensionPriority = std::pair<int, std::string>;
    std::vector<ExtensionPriority> priority;
    for (const auto &[tid, type] : m_asset_types) {
        priority.emplace_back(type.load_priority, type.file_extension);
    }
    std::sort(
        priority.begin(), priority.end(),
        [](const auto &lhs, const auto &rhs) { return lhs.first < rhs.first; });

    // scan specify extension
    for (const auto &[priorty, ext] : priority) {
        ScanDirectory(dir, ext);
    }
}

}  // namespace SD
