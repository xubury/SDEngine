#include "Graphics/OpenGL/GLVertexArray.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"

namespace SD {

GLVertexArray::GLVertexArray() : m_id(0) { glCreateVertexArrays(1, &m_id); }

GLVertexArray::~GLVertexArray() { glDeleteVertexArrays(1, &m_id); }

void GLVertexArray::Bind() const { glBindVertexArray(m_id); }

void GLVertexArray::AddVertexBuffer(const Ref<VertexBuffer> &buffer,
                                    const VertexBufferLayout &layout,
                                    int index) {
    size_t offset = 0;
    uint32_t i = index > 0 ? index : m_vertexBuffers.size();
    for (const auto &element : layout.GetElements()) {
        glVertexArrayVertexBuffer(m_id, i, buffer->GetId(), 0,
                                  layout.GetStride());
        glVertexArrayBindingDivisor(m_id, i, layout.GetInstanceStride());
        glVertexArrayAttribBinding(m_id, i, i);

        glEnableVertexArrayAttrib(m_id, i);
        switch (element.type) {
            case BufferDataType::FLOAT:
            case BufferDataType::FLOAT2:
            case BufferDataType::FLOAT3:
            case BufferDataType::FLOAT4:
                glVertexArrayAttribFormat(m_id, i, element.count,
                                          Translate(element.type),
                                          element.normalized, offset);
                break;
            case BufferDataType::UCHAR:
            case BufferDataType::UINT:
                glVertexArrayAttribIFormat(m_id, i, element.count,
                                           Translate(element.type), offset);

                break;
        }
        offset += GetBufferDataSize(element.type);
        ++i;
    }
    m_vertexBuffers.push_back(buffer);
    m_layouts.push_back(layout);
}

void GLVertexArray::UpdateBuffer(size_t index, const void *data, size_t size,
                                 size_t offset) {
    m_vertexBuffers[index]->UpdateData(data, size, offset);
}

void GLVertexArray::SetIndexBuffer(const Ref<IndexBuffer> &buffer) {
    glVertexArrayElementBuffer(
        m_id, std::static_pointer_cast<GLIndexBuffer>(buffer)->GetId());
    m_indexBuffer = buffer;
}

Ref<IndexBuffer> GLVertexArray::GetIndexBuffer() { return m_indexBuffer; }

}  // namespace SD
