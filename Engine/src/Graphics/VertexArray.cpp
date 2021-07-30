#include "Graphics/VertexArray.hpp"
#include "Core/Assert.hpp"

namespace sd {

Ref<VertexArray> VertexArray::create() { return createRef<VertexArray>(); }

VertexArray::VertexArray() : m_id(0) { glGenVertexArrays(1, &m_id); }

VertexArray::~VertexArray() { glDeleteVertexArrays(1, &m_id); }

void VertexArray::bind() const { glBindVertexArray(m_id); }

void VertexArray::unbind() const { glBindVertexArray(0); }

void VertexArray::addVertexBuffer(const Ref<VertexBuffer> &buffer,
                                  const VertexBufferLayout &layout, int index) {
    buffer->bind();
    bind();

    size_t offset = 0;
    uint32_t i = index > 0 ? index : m_vertexBuffers.size();
    for (const VertexBufferLayoutElement &element : layout.getElements()) {
        glEnableVertexAttribArray(i);
        glVertexAttribDivisor(i, layout.getInstanceStride());
        glVertexAttribPointer(i, element.count, element.type,
                              element.normalized, layout.getStride(),
                              (const void *)offset);
        offset += element.count * getSizeOfType(element.type);
        ++i;
    }
    buffer->unbind();
    unbind();

    m_vertexBuffers.push_back(buffer);
}

void VertexArray::setIndexBuffer(const Ref<IndexBuffer> &buffer) {
    bind();
    buffer->bind();
    m_indexBuffer = buffer;
}

void VertexArray::updateBuffer(size_t index, const void *data, size_t size,
                               size_t offset) {
    m_vertexBuffers[index]->updateData(data, size, offset);
}

}  // namespace sd
