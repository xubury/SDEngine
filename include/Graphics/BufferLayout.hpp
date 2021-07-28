#ifndef SD_BUFFER_LAYEROUT_HPP
#define SD_BUFFER_LAYEROUT_HPP

#include "Core/Export.hpp"
#include <string>
#include <vector>

namespace sd {

enum class ShaderDataType {
    None = 0,
    Float,
    Float2,
    Float3,
    Float4,
    Mat3,
    Mat4,
    Int,
    Int2,
    Int3,
    Int4,
    Bool
};

class SD_API BufferElement {
   public:
    BufferElement() = default;

    BufferElement(ShaderDataType type, const std::string &name,
                  bool normalized = false);

    uint32_t getComponentCount() const;

    ShaderDataType getType() const;

    uint32_t getSize() const;

    size_t getOffset() const;

    bool isNormalized() const;

   private:
    friend class BufferLayout;

    ShaderDataType m_type;
    std::string m_name;
    uint32_t m_size;
    size_t m_offset;
    bool m_normalized;
};

class SD_API BufferLayout {
   public:
    BufferLayout() = default;

    BufferLayout(std::initializer_list<BufferElement> elements);

    std::vector<BufferElement>::iterator begin();

    std::vector<BufferElement>::iterator end();

    std::vector<BufferElement>::const_iterator begin() const;

    std::vector<BufferElement>::const_iterator end() const;

    size_t getStride() const;

   private:
    void calculateOffsetAndStride();

    std::vector<BufferElement> m_elements;

    size_t m_stride;
};

}  // namespace sd
#endif /* SD_BUFFER_LAYEROUT_HPP */
