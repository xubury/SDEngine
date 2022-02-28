#include "Asset/AssetStorage.hpp"

namespace SD {

void AssetStorage::Init() {}

void AssetStorage::Shutdown() {
    for (auto &[res_type, cache] : m_assets) {
        for (auto &[sid, ptr] : cache) {
            GetTypeData(res_type).m_destroy_func(ptr);
        }
        cache.clear();
    }
    m_assets.clear();
}

}  // namespace SD
