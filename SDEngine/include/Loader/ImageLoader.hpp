#ifndef SD_IMAGE_LOADER_HPP
#define SD_IMAGE_LOADER_HPP

#include "Loader/Export.hpp"
#include "Graphics/Image.hpp"

namespace SD {

class SD_LOADER_API ImageLoader {
   public:
    Ref<ByteImage> Load(const std::string &path);
    Ref<ByteImage> Load(const uint8_t *data, int32_t size);
};

}  // namespace SD
#endif
