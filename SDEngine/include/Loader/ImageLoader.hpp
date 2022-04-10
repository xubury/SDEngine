#ifndef SD_IMAGE_LOADER_HPP
#define SD_IMAGE_LOADER_HPP

#include "Loader/Export.hpp"
#include "Graphics/Bitmap.hpp"

namespace SD {

class SD_LOADER_API ImageLoader {
   public:
    static Ref<Bitmap> Load(const std::string &path);
};

}  // namespace SD
#endif
