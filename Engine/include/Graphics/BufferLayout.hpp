#ifndef SD_BUFFER_LAYEROUT_HPP
#define SD_BUFFER_LAYEROUT_HPP

#include "Core/Export.hpp"
#include "Core/Assert.hpp"
#include <string>
#include <vector>
#include <GL/glew.h>

namespace sd {

struct SD_API VertexBufferLayoutElement {
    GLenum type;
    uint32_t count;
    uint8_t normalized;
};

uint32_t getSizeOfType(GLenum type);

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
    m_elements.push_back({GL_FLOAT, count, GL_FALSE});
    m_stride += count * getSizeOfType(GL_FLOAT);
}

template <>
inline void VertexBufferLayout::push<uint32_t>(uint32_t count) {
    m_elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
    m_stride += count * getSizeOfType(GL_UNSIGNED_INT);
}

template <>
inline void VertexBufferLayout::push<uint8_t>(uint32_t count) {
    m_elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
    m_stride += count * getSizeOfType(GL_UNSIGNED_BYTE);
}
}  // namespace sd

#endif /* SD_BUFFER_LAYEROUT_HPP */
