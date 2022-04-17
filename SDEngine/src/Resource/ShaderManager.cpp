#include "Resource/ShaderManager.hpp"

namespace SD {

ShaderManager &ShaderManager::Get()
{
    static ShaderManager s_instance;
    return s_instance;
}

void ShaderManager::Init(std::filesystem::path directory)

{
    m_directory = std::move(directory);
}

}  // namespace SD
