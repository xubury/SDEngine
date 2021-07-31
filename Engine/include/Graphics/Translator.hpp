#ifndef SD_TRANSLATOR_HPP
#define SD_TRANSLATOR_HPP

#include "Core/Base.hpp"
#include "Graphics/RendererAPI.hpp"

namespace sd {

template <typename INT, typename BOOL, typename ENUM>
class Translator {
   public:
    virtual BOOL translate(Boolean dataType) = 0;

    virtual ENUM translate(DataType dataType) = 0;

    virtual ENUM translate(MeshTopology meshType) = 0;

    virtual INT translate(BufferType bufferType) = 0;

    virtual INT translate(BufferIOType ioType) = 0;
};

}  // namespace sd

#endif /* SD_TRANSLATOR_HPP */
