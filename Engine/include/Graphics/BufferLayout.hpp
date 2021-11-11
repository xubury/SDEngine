#ifndef SD_BUFFER_LAYEROUT_HPP
#define SD_BUFFER_LAYEROUT_HPP

#include <string>
#include <vector>
#include "Utility/Export.hpp"
#include "Utility/Assert.hpp"
#include "Graphics/Graphics.hpp"

namespace SD {

struct SD_API VertexBufferLayoutElement {
    BufferDataType type;
    uint32_t count;
    bool normalized;
};

uint32_t getSizeOfType(BufferDataType type);

class SD_API VertexBufferLayout {
   public:
    VertexBufferLayout(unsigned instanceStride = 0);

    void push(BufferDataType type, uint32_t count, bool normalized = false);

    void clear();

    const std::vector<VertexBufferLayoutElement>& getElements() const;

    uint32_t getStride() const;

    uint32_t getInstanceStride() const;

   private:
    std::vector<VertexBufferLayoutElement> m_elements;
    uint32_t m_stride;
    uint32_t m_instanceStride;
};

inline VertexBufferLayout makeLayout(BufferDataType type, uint32_t count,
                                     uint32_t instanceStride = 0) {
    VertexBufferLayout layout(instanceStride);
    layout.push(type, count);
    return layout;
}

}  // namespace SD

#endif /* SD_BUFFER_LAYEROUT_HPP */
