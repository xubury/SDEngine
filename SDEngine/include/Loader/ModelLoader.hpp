#ifndef SD_MODEL_LOADER_HPP
#define SD_MODEL_LOADER_HPP

#include "Loader/Export.hpp"
#include "Resource/ResourceLoader.hpp"
#include "Graphics/Model.hpp"
#include "Utility/Base.hpp"

#include <string>

namespace SD {

class ResourceRegistry;

class SD_LOADER_API ModelLoader {
   public:
    Ref<Model> Load(const std::string &path, ResourceRegistry &registry);
};

}  // namespace SD

#endif /* SD_MODEL_LOADER_HPP */
