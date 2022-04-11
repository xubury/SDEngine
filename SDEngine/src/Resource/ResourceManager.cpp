#include "Resource/ResourceManager.hpp"
#include <fstream>

namespace SD {

const std::string meta_name = "resource.meta";

ResourceManager &ResourceManager::Get()
{
    static ResourceManager s_instance;
    return s_instance;
}

void ResourceManager::Init(const std::string &path)
{
    m_directory = path;
    const std::string filename = std::filesystem::path(path) / meta_name;
    if (std::filesystem::exists(filename)) {
        std::ifstream is(filename, std::ios::binary);
        cereal::XMLInputArchive archive(is);
        archive(m_resources, m_path_ids);
    }
    m_is_modified = false;
    // for (auto &[tid, caches] : m_resources) {
    //     auto &loader = m_serializer[tid];
    //     for (auto &[rid, cache] : caches) {
    //         cache.data = loader(m_directory / cache.path);
    //     }
    // }
}

void ResourceManager::ShutDown()
{
    if (m_is_modified) {
        const std::string filename =
            std::filesystem::path(m_directory) / meta_name;
        std::ofstream os(filename, std::ios::binary);
        cereal::XMLOutputArchive archive(os);
        archive(m_resources, m_path_ids);
    }
}

}  // namespace SD
