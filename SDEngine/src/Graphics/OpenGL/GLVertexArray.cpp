#include "Graphics/OpenGL/GLVertexArray.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Utility/Log.hpp"

#include <numeric>

namespace SD {

GLVertexArray::GLVertexArray() : m_attrib_id(0) {
    glCreateVertexArrays(1, &m_id);
}

GLVertexArray::~GLVertexArray() { glDeleteVertexArrays(1, &m_id); }

void GLVertexArray::Bind() const { glBindVertexArray(m_id); }

void GLVertexArray::AddVertexBuffer(const Ref<VertexBuffer> &buffer,
                                    const VertexBufferLayout &layout) {
    int binding_index = m_vertex_buffers.size();
    glVertexArrayVertexBuffer(m_id, binding_index, buffer->GetId(), 0,
                              layout.GetStride());
    glVertexArrayBindingDivisor(m_id, binding_index,
                                layout.GetInstanceStride());
    for (const auto &element : layout.GetElements()) {
        switch (element.type) {
            case BufferLayoutType::FLOAT:
            case BufferLayoutType::FLOAT2:
            case BufferLayoutType::FLOAT3:
            case BufferLayoutType::FLOAT4: {
                glEnableVertexArrayAttrib(m_id, m_attrib_id);
                glVertexArrayAttribBinding(m_id, m_attrib_id, binding_index);
                glVertexArrayAttribFormat(m_id, m_attrib_id, element.count,
                                          Translate(element.type),
                                          element.normalized, element.offset);
                ++m_attrib_id;
            } break;
            case BufferLayoutType::MAT4: {
                for (int i = 0; i < 4; ++i) {
                    glEnableVertexArrayAttrib(m_id, m_attrib_id);
                    glVertexArrayAttribBinding(m_id, m_attrib_id,
                                               binding_index);
                    glVertexArrayAttribFormat(
                        m_id, m_attrib_id, element.count,
                        Translate(element.type), element.normalized,
                        element.offset + sizeof(float) * 4 * i);
                    ++m_attrib_id;
                }
            } break;
            case BufferLayoutType::UBYTE:
            case BufferLayoutType::UINT:
            case BufferLayoutType::INT: {
                glVertexArrayVertexBuffer(m_id, binding_index, buffer->GetId(),
                                          0, layout.GetStride());
                glEnableVertexArrayAttrib(m_id, m_attrib_id);
                glVertexArrayAttribBinding(m_id, m_attrib_id, binding_index);
                glVertexArrayAttribIFormat(m_id, m_attrib_id, element.count,
                                           Translate(element.type),
                                           element.offset);

                ++m_attrib_id;
            } break;
        }
    }
    m_vertex_buffers.push_back(buffer);
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
