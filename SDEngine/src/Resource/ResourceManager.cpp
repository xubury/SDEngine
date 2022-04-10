#include "Resource/ResourceManager.hpp"

namespace SD {


ResourceManager& ResourceManager::Get()
{
    static ResourceManager s_instance;
    return s_instance;
}

}  // namespace SD
