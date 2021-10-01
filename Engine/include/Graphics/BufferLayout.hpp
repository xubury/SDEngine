#ifndef SD_BUFFER_LAYEROUT_HPP
#define SD_BUFFER_LAYEROUT_HPP

#include <string>
#include <vector>
#include "Utility/Export.hpp"
#include "Utility/Assert.hpp"
#include "Graphics/Graphics.hpp"

namespace sd {

struct SD_API VertexBufferLayoutElement {
    BufferDataType type;
    uint32_t count;
    bool normalized;
};

uint32_t getSizeOfType(BufferDataType type);

class SD_API VertexBufferLayout {
   public:
    VertexBufferLayout(unsigned instanceStride = 0);

    template <typename T>
    void push(uint32_t) {
        SD_CORE_ERROR("Cannot push invalid buffer type!");
        SD_CORE_ASSERT(false, "Cannot push invalid buffer type!");
    }

    void clear();

    const std::vector<VertexBufferLayoutElement>& getElements() const;

    uint32_t getStride() const;

    uint32_t getInstanceStride() const;

   private:
    std::vector<VertexBufferLayoutElement> m_elements;
    uint32_t m_stride;
    uint32_t m_instanceStride;
};

template <typename T>
VertexBufferLayout makeLayout(uint32_t count, uint32_t instanceStride = 0) {
    VertexBufferLayout layout(instanceStride);
    layout.push<T>(count);

    return layout;
}

template <>
inline void VertexBufferLayout::push<float>(uint32_t count) {
    m_elements.push_back({BufferDataType::FLOAT, count, false});
    m_stride += count * getSizeOfType(BufferDataType::FLOAT);
}

template <>
inline void VertexBufferLayout::push<uint32_t>(uint32_t count) {
    m_elements.push_back({BufferDataType::UINT, count, false});
    m_stride += count * getSizeOfType(BufferDataType::UINT);
}

template <>
inline void VertexBufferLayout::push<uint8_t>(uint32_t count) {
    m_elements.push_back({BufferDataType::UCHAR, count, true});
    m_stride += count * getSizeOfType(BufferDataType::UCHAR);
}
}  // namespace sd

#endif /* SD_BUFFER_LAYEROUT_HPP */
