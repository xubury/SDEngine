#include "Resource/ResourceManager.hpp"
#include <fstream>

namespace SD {

const std::string meta_name = "resource.meta";

ResourceManager &ResourceManager::Get()
{
    static ResourceManager s_instance;
    return s_instance;
}

void ResourceManager::Init(const std::filesystem::path &path)
{
    m_directory = path;
    const std::filesystem::path filename = m_directory / meta_name;
    if (std::filesystem::exists(filename)) {
        std::ifstream is(filename, std::ios::binary);
        cereal::XMLInputArchive archive(is);
        archive(m_caches, m_path_ids);
    }
    m_is_modified = false;
}

void ResourceManager::ShutDown()
{
    if (m_is_modified) {
        const std::filesystem::path filename = m_directory / meta_name;
        std::ofstream os(filename, std::ios::binary);
        cereal::XMLOutputArchive archive(os);
        archive(m_caches, m_path_ids);
    }
    m_caches.clear();
    m_path_ids.clear();
    m_resource_types.clear();
}

}  // namespace SD
