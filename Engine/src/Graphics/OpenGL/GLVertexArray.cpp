#include "Graphics/OpenGL/GLVertexArray.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Utils/Assert.hpp"

namespace sd {

GLVertexArray::GLVertexArray() : m_id(0) {}

GLVertexArray::~GLVertexArray() {
    if (m_id != 0) glDeleteVertexArrays(1, &m_id);
}

void GLVertexArray::init() {
    if (m_id != 0) glDeleteVertexArrays(1, &m_id);
    glGenVertexArrays(1, &m_id);
}

void GLVertexArray::bind() const { glBindVertexArray(m_id); }

void GLVertexArray::unbind() const { glBindVertexArray(0); }

void GLVertexArray::addVertexBuffer(const Ref<VertexBuffer> &buffer,
                                    const VertexBufferLayout &layout,
                                    int index) {
    buffer->bind();
    bind();

    size_t offset = 0;
    uint32_t i = index > 0 ? index : m_vertexBuffers.size();
    for (const VertexBufferLayoutElement &element : layout.getElements()) {
        glEnableVertexAttribArray(i);
        glVertexAttribDivisor(i, layout.getInstanceStride());
        switch (element.type) {
            case BufferDataType::FLOAT:
                glVertexAttribPointer(i, element.count, TRANSLATE(element.type),
                                      element.normalized, layout.getStride(),
                                      (const void *)offset);
                break;
            case BufferDataType::UCHAR:
            case BufferDataType::UINT:
                glVertexAttribIPointer(
                    i, element.count, TRANSLATE(element.type),
                    layout.getStride(), (const void *)offset);
                break;
        }
        offset += element.count * getSizeOfType(element.type);
        ++i;
    }
    buffer->unbind();
    unbind();

    m_vertexBuffers.push_back(buffer);
}

void GLVertexArray::setIndexBuffer(const Ref<IndexBuffer> &buffer) {
    bind();
    buffer->bind();
    m_indexBuffer = buffer;
}

void GLVertexArray::updateBuffer(size_t index, const void *data, size_t size,
                                 size_t offset) {
    m_vertexBuffers[index]->updateData(data, size, offset);
}

}  // namespace sd
