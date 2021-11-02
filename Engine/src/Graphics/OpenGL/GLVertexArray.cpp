#include "Graphics/OpenGL/GLVertexArray.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Utility/Assert.hpp"

namespace sd {

GLVertexArray::GLVertexArray() : m_id(0) { glCreateVertexArrays(1, &m_id); }

GLVertexArray::~GLVertexArray() { glDeleteVertexArrays(1, &m_id); }

void GLVertexArray::bind() const { glBindVertexArray(m_id); }

void GLVertexArray::unbind() const { glBindVertexArray(0); }

void GLVertexArray::addVertexBuffer(const Ref<VertexBuffer> &buffer,
                                    const VertexBufferLayout &layout,
                                    int index) {
    size_t offset = 0;
    auto glbuffer = std::static_pointer_cast<GLVertexBuffer>(buffer);
    uint32_t i = index > 0 ? index : m_vertexBuffers.size();
    for (const auto &element : layout.getElements()) {
        glVertexArrayVertexBuffer(m_id, i, glbuffer->getId(), 0,
                                  layout.getStride());
        glVertexArrayBindingDivisor(m_id, i, layout.getInstanceStride());
        glVertexArrayAttribBinding(m_id, i, i);

        glEnableVertexArrayAttrib(m_id, i);
        switch (element.type) {
            case BufferDataType::FLOAT:
            case BufferDataType::FLOAT2:
            case BufferDataType::FLOAT3:
            case BufferDataType::FLOAT4:
                glVertexArrayAttribFormat(m_id, i, element.count,
                                          translate(element.type),
                                          element.normalized, offset);
                break;
            case BufferDataType::UCHAR:
            case BufferDataType::UINT:
                glVertexArrayAttribIFormat(m_id, i, element.count,
                                           translate(element.type), offset);

                break;
        }
        offset += element.count * getSizeOfType(element.type);
        ++i;
    }
    m_vertexBuffers.push_back(buffer);
}

void GLVertexArray::updateBuffer(size_t index, const void *data, size_t size,
                                 size_t offset) {
    m_vertexBuffers[index]->updateData(data, size, offset);
}

void GLVertexArray::setIndexBuffer(const Ref<IndexBuffer> &buffer) {
    glVertexArrayElementBuffer(
        m_id, std::static_pointer_cast<GLIndexBuffer>(buffer)->getId());
    m_indexBuffer = buffer;
}

Ref<IndexBuffer> GLVertexArray::getIndexBuffer() { return m_indexBuffer; }

}  // namespace sd
