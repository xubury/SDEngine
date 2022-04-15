#include "ECS/ResourceRegistry.hpp"

namespace SD {

void ResourceRegistry::SetDirectory(std::filesystem::path&& directory)
{
    m_directory = std::move(directory);
}

}  // namespace SD
