#include "Graphics/OpenGL/GLVertexArray.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"
#include "Utility/Log.hpp"

#include <numeric>

namespace SD {

GLVertexArray::GLVertexArray() : m_attrib_id(0)
{
    glCreateVertexArrays(1, &m_id);
}

GLVertexArray::~GLVertexArray() { glDeleteVertexArrays(1, &m_id); }

void GLVertexArray::BindVertexBuffer(const VertexBuffer &buffer, int32_t index)
{
    glVertexArrayVertexBuffer(m_id, index, buffer.Handle(), 0,
                              m_layouts[index].GetStride());
}

void GLVertexArray::AddBufferLayout(const VertexBufferLayout &layout)
{
    int binding_index = m_layouts.size();
    glVertexArrayBindingDivisor(m_id, binding_index,
                                layout.GetInstanceStride());
    for (const auto &element : layout.GetElements()) {
        switch (element.type) {
            case BufferLayoutType::Float:
            case BufferLayoutType::Float2:
            case BufferLayoutType::Float3:
            case BufferLayoutType::Float4: {
                glEnableVertexArrayAttrib(m_id, m_attrib_id);
                glVertexArrayAttribBinding(m_id, m_attrib_id, binding_index);
                glVertexArrayAttribFormat(m_id, m_attrib_id, element.count,
                                          Translate(element.type),
                                          element.normalized, element.offset);
                ++m_attrib_id;
            } break;
            case BufferLayoutType::Mat4: {
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
            case BufferLayoutType::UByte:
            case BufferLayoutType::UInt:
            case BufferLayoutType::Int: {
                glEnableVertexArrayAttrib(m_id, m_attrib_id);
                glVertexArrayAttribBinding(m_id, m_attrib_id, binding_index);
                glVertexArrayAttribIFormat(m_id, m_attrib_id, element.count,
                                           Translate(element.type),
                                           element.offset);

                ++m_attrib_id;
            } break;
        }
    }
    m_layouts.push_back(layout);
}

void GLVertexArray::BindIndexBuffer(const IndexBuffer &buffer)
{
    glVertexArrayElementBuffer(m_id, buffer.Handle());
}

}  // namespace SD
