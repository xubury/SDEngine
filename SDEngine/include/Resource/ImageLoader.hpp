#ifndef SD_IMAGE_LOADER_HPP
#define SD_IMAGE_LOADER_HPP

#include "Resource/Export.hpp"
#include "Graphics/Image.hpp"

namespace SD {

class SD_RESOURCE_API ImageLoader {
   public:
    Ref<ByteImage> Load(const std::string &path);
    Ref<ByteImage> Load(const std::string_view &path);
    Ref<ByteImage> Load(const uint8_t *data, int32_t size);
};

}  // namespace SD
#endif
