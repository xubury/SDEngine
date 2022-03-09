#include "Asset/AssetStorage.hpp"

namespace SD {

const std::string AssetStorage::ASSET_IDENTIFIER = "SD_ASSET";
const std::string AssetStorage::ASSET_POSFIX = ".sda";

static AssetStorage *s_storage = nullptr;

AssetStorage &AssetStorage::Get() { return *s_storage; }

void AssetStorage::Init() { s_storage = new AssetStorage; }

void AssetStorage::Shutdown() {
    delete s_storage;
    s_storage = nullptr;
}

AssetStorage::~AssetStorage() {
    for (auto &[res_type, cache] : m_assets) {
        for (auto &[sid, ptr] : cache) {
            GetTypeData(res_type).destroy_func(ptr);
        }
        cache.clear();
    }
    m_assets.clear();
}

void AssetStorage::ScanDirectory(const std::filesystem::path &dir) {
    for (auto &entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_directory()) {
            ScanDirectory(entry);
        } else if (entry.is_regular_file() &&
                   entry.path().extension() == ASSET_POSFIX) {
            LoadAsset(entry.path().generic_string());
        }
    }
}

}  // namespace SD
