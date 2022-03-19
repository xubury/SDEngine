#ifndef SD_MODEL_LOADER_HPP
#define SD_MODEL_LOADER_HPP

#include "Loader/Export.hpp"
#include "Graphics/Model.hpp"
#include "Utility/Base.hpp"

#include <string>

namespace SD {

class SD_LOADER_API ModelLoader {
   public:
    static Ref<Model> LoadModel(const std::string &path);
};

}  // namespace SD

#endif /* SD_MODEL_LOADER_HPP */
