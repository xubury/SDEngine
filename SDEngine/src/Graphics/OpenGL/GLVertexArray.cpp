#include "Graphics/OpenGL/GLVertexArray.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Utility/Log.hpp"

#include <numeric>

namespace SD {

GLVertexArray::GLVertexArray() : m_id(0), m_attrib_id(0) {
    glCreateVertexArrays(1, &m_id);
}

GLVertexArray::~GLVertexArray() { glDeleteVertexArrays(1, &m_id); }

void GLVertexArray::Bind() const { glBindVertexArray(m_id); }

void GLVertexArray::AddVertexBuffer(const Ref<VertexBuffer> &buffer,
                                    const VertexBufferLayout &layout) {
    size_t offset = 0;
    for (const auto &element : layout.GetElements()) {
        glVertexArrayVertexBuffer(m_id, m_attrib_id, buffer->GetId(), 0,
                                  layout.GetStride());
        glVertexArrayBindingDivisor(m_id, m_attrib_id,
                                    layout.GetInstanceStride());
        glVertexArrayAttribBinding(m_id, m_attrib_id, m_attrib_id);

        glEnableVertexArrayAttrib(m_id, m_attrib_id);
        switch (element.type) {
            case BufferLayoutType::FLOAT:
            case BufferLayoutType::FLOAT2:
            case BufferLayoutType::FLOAT3:
            case BufferLayoutType::FLOAT4:
                glVertexArrayAttribFormat(m_id, m_attrib_id, element.count,
                                          Translate(element.type),
                                          element.normalized, offset);
                break;
            case BufferLayoutType::UBYTE:
            case BufferLayoutType::UINT:
            case BufferLayoutType::INT:
                glVertexArrayAttribIFormat(m_id, m_attrib_id, element.count,
                                           Translate(element.type), offset);

                break;
        }
        offset += GetBufferDataSize(element.type);
        ++m_attrib_id;
    }
    m_vertex_buffers.push_back(buffer);
    m_layouts.push_back(layout);
}

void GLVertexArray::UpdateBuffer(size_t index, const void *data, size_t size,
                                 size_t offset) {
    m_vertex_buffers[index]->UpdateData(data, size, offset);
}

void GLVertexArray::SetIndexBuffer(const Ref<IndexBuffer> &buffer) {
    glVertexArrayElementBuffer(
        m_id, std::static_pointer_cast<GLIndexBuffer>(buffer)->GetId());
    m_indexBuffer = buffer;
}

Ref<IndexBuffer> GLVertexArray::GetIndexBuffer() { return m_indexBuffer; }

}  // namespace SD
