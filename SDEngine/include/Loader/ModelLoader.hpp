#ifndef SD_MODEL_LOADER_HPP
#define SD_MODEL_LOADER_HPP

#include "Loader/Export.hpp"
#include "Resource/ResourceCache.hpp"
#include "Resource/ResourceLoader.hpp"
#include "Graphics/Model.hpp"
#include "Utility/Base.hpp"

#include <string>

namespace SD {

class SD_LOADER_API ModelLoader : public ResourceLoader<ModelLoader, Model> {
   public:
    Ref<Model> Load(const std::string &path,
                    ResourceCache<Texture> &cache) const;
};

}  // namespace SD

#endif /* SD_MODEL_LOADER_HPP */
