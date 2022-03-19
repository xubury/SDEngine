#include "Graphics/OpenGL/GLBuffer.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"

namespace SD {

GLBuffer::GLBuffer(GLenum type, GLenum io, const void *data, size_t size)
    : m_type(type), m_io(io), m_size(size)
{
    glCreateBuffers(1, &m_id);
    glNamedBufferData(m_id, size, data, m_io);
}

GLBuffer::~GLBuffer() { glDeleteBuffers(1, &m_id); }

void GLBuffer::UpdateData(const void *data, size_t size, size_t offset)
{
    if (size > m_size) {
        m_size = size;
        glNamedBufferData(m_id, size, data, m_io);
    }
    else {
        if (size == 0) {
            size = m_size;
        }
        glNamedBufferSubData(m_id, offset, size, data);
    }
}

GLVertexBuffer::GLVertexBuffer(const void *data, size_t size, BufferIOType io)
    : GLBuffer(GL_ARRAY_BUFFER, Translate(io), data, size)
{
}

GLIndexBuffer::GLIndexBuffer(const uint32_t *data, uint32_t count,
                             BufferIOType io)
    : IndexBuffer(count),
      GLBuffer(GL_ELEMENT_ARRAY_BUFFER, Translate(io), data,
               count * sizeof(uint32_t))
{
}

void GLIndexBuffer::UpdateData(const void *data, size_t size, size_t offset)
{
    GLBuffer::UpdateData(data, size, offset);
    m_count = size / sizeof(uint32_t);
}

uint32_t GLUniformBuffer::s_count = 0;

GLUniformBuffer::GLUniformBuffer(const void *data, size_t size, BufferIOType io)
    : GLBuffer(GL_UNIFORM_BUFFER, Translate(io), data, size)
{
    m_base = s_count++;
    glBindBufferBase(m_type, m_base, m_id);
}

uint32_t GLUniformBuffer::GetBindingPoint() const { return m_base; }

}  // namespace SD
