#include "Asset/AssetStorage.hpp"

namespace SD {

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

}  // namespace SD
