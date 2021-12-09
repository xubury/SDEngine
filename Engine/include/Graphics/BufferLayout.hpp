#ifndef SD_BUFFER_LAYEROUT_HPP
#define SD_BUFFER_LAYEROUT_HPP

#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"

#include <string>
#include <vector>

namespace SD {

struct SD_GRAPHICS_API VertexBufferLayoutElement {
    BufferLayoutType type;
    uint32_t count;
    bool normalized;
};

uint32_t SD_GRAPHICS_API GetBufferDataSize(BufferLayoutType type);

uint32_t SD_GRAPHICS_API GetComponentCount(BufferLayoutType type);

class SD_GRAPHICS_API VertexBufferLayout {
   public:
    VertexBufferLayout(uint32_t instance_stride = 0);

    void Push(BufferLayoutType type, bool normalized = false);

    void Clear();

    const std::vector<VertexBufferLayoutElement>& GetElements() const;

    uint32_t GetStride() const;

    uint32_t GetInstanceStride() const;

   private:
    std::vector<VertexBufferLayoutElement> m_elements;
    uint32_t m_stride;
    uint32_t m_instance_stride;
};

}  // namespace SD

#endif /* SD_BUFFER_LAYEROUT_HPP */
