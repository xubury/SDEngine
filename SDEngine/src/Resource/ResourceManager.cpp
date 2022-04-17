#include "Resource/ResourceManager.hpp"

namespace SD {

ResourceManager &ResourceManager::Get()
{
    static ResourceManager s_instance;
    return s_instance;
}

void ResourceManager::Init(std::filesystem::path directory)
{
    m_directory = std::move(directory);
}

}  // namespace SD
