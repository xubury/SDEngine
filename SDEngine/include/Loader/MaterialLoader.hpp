#ifndef SD_MATERIAL_LOADER_HPP
#define SD_MATERIAL_LOADER_HPP

#include "Loader/Export.hpp"
#include "Graphics/Material.hpp"

namespace SD {

class SD_LOADER_API MaterialLoader {
   public:
    static Ref<Material> Load(const std::string &path);
    static void Save(const Material &material, const std::string &path);
};

}  // namespace SD
#endif
