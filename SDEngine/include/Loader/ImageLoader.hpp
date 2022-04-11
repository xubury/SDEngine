#ifndef SD_IMAGE_LOADER_HPP
#define SD_IMAGE_LOADER_HPP

#include "Loader/Export.hpp"
#include "Graphics/Image.hpp"

namespace SD {

class SD_LOADER_API ImageLoader {
   public:
    static Ref<ByteImage> LoadFromFile(const std::string &path);
    static Ref<ByteImage> LoadFromMemory(const uint8_t *data, int32_t size);
};

}  // namespace SD
#endif
