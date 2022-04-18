#ifndef SD_MODEL_LOADER_HPP
#define SD_MODEL_LOADER_HPP

#include "Resource/Export.hpp"
#include "Graphics/Model.hpp"
#include "Utility/Base.hpp"

#include <string>

namespace SD {

class SD_RESOURCE_API ModelLoader {
   public:
    Ref<Model> Load(const std::string_view &path);
};

}  // namespace SD

#endif /* SD_MODEL_LOADER_HPP */
