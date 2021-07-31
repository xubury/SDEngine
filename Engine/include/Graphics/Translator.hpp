#ifndef SD_TRANSLATOR_HPP
#define SD_TRANSLATOR_HPP

#include "Core/Base.hpp"
#include "Graphics/RendererAPI.hpp"

namespace sd {

template <typename INT, typename BOOL, typename ENUM>
class Translator {
   public:
    virtual ENUM translate(BufferDataType dataType) = 0;

    virtual ENUM translate(MeshTopology meshType) = 0;

    virtual INT translate(BufferIOType ioType) = 0;

    virtual ENUM translate(TextureType textureType) = 0;

    virtual ENUM translate(TextureFormat textureFormat) = 0;

    virtual ENUM translate(TextureFormatType textureFormatType) = 0;

    virtual INT translate(TextureFormat textureFormat,
                          TextureFormatType textureFormatType) = 0;

    virtual INT translate(TextureWrap textureWrap) = 0;

    virtual INT translate(TextureFilter textureWrap) = 0;

    virtual INT translate(TextureMipmapFilter textureWrap) = 0;
};

}  // namespace sd

#endif /* SD_TRANSLATOR_HPP */
