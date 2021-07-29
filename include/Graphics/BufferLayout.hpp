#ifndef SD_BUFFER_LAYEROUT_HPP
#define SD_BUFFER_LAYEROUT_HPP

#include "Core/Export.hpp"
#include "Core/Assert.hpp"
#include <string>
#include <vector>
#include <GL/gl.h>

namespace sd {

struct SD_API VertexBufferLayoutElement {
    GLenum type;
    uint32_t count;
    uint8_t normalized;
};

class SD_API VertexBufferLayout {
   public:
    VertexBufferLayout(unsigned instanceStride = 0);

    template <typename _t>
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

uint32_t getSizeOfType(GLenum type);

}  // namespace sd

#endif /* SD_BUFFER_LAYEROUT_HPP */
