#include <GL/glew.h>
#include "Graphics/VertexArray.hpp"
#include "Core/Assert.hpp"

namespace sd {

VertexArray::VertexArray() : m_id(0), m_vertexBufferId(0) {
    glGenVertexArrays(1, &m_id);
}

VertexArray::~VertexArray() { glDeleteVertexArrays(1, &m_id); }

void VertexArray::bind() const { glBindVertexArray(m_id); }

void VertexArray::unbind() const { glBindVertexArray(0); }

void VertexArray::addVertexBuffer(const Ref<VertexBuffer> &buffer,
                                  const VertexBufferLayout &layout, int index) {
    bind();
    buffer->bind();

    size_t offset = 0;
    uint32_t i = index > 0 ? index : m_buffers.size();
    for (const VertexBufferLayoutElement &element : layout.getElements()) {
        glEnableVertexAttribArray(i);
        glVertexAttribDivisor(i, layout.getInstanceStride());
        glVertexAttribPointer(i, element.count, element.type,
                              element.normalized, layout.getStride(),
                              (const void *)offset);
        offset += element.count * getSizeOfType(element.type);
    }
    unbind();
    buffer->unbind();
    m_buffers.push_back(buffer);
}

void VertexArray::updateBuffer(size_t index, const void *data, size_t size,
                               size_t offset) {
    m_buffers[index]->updateData(data, size, offset);
}

}  // namespace sd
