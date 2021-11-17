#ifndef SD_BUFFER_LAYEROUT_HPP
#define SD_BUFFER_LAYEROUT_HPP

#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"

#include <string>
#include <vector>

namespace SD {

struct SD_API VertexBufferLayoutElement {
    BufferDataType type;
    uint32_t count;
    bool normalized;
};

uint32_t GetSizeOfType(BufferDataType type);

class SD_API VertexBufferLayout {
   public:
    VertexBufferLayout(unsigned instanceStride = 0);

    void Push(BufferDataType type, uint32_t count, bool normalized = false);

    void Clear();

    const std::vector<VertexBufferLayoutElement>& GetElements() const;

    uint32_t GetStride() const;

    uint32_t GetInstanceStride() const;

   private:
    std::vector<VertexBufferLayoutElement> m_elements;
    uint32_t m_stride;
    uint32_t m_instanceStride;
};

inline VertexBufferLayout MakeLayout(BufferDataType type, uint32_t count,
                                     uint32_t instanceStride = 0) {
    VertexBufferLayout layout(instanceStride);
    layout.Push(type, count);
    return layout;
}

}  // namespace SD

#endif /* SD_BUFFER_LAYEROUT_HPP */
