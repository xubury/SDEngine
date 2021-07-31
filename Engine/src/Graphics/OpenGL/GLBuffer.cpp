#include "Graphics/OpenGL/GLBuffer.hpp"
#include "Graphics/OpenGL/GLTranslator.hpp"

namespace sd {

GLBuffer::GLBuffer(GLenum type, GLenum io, const void *data, size_t size)
    : m_type(type), m_io(io), m_data(data), m_size(size) {}

GLBuffer::~GLBuffer() { glDeleteBuffers(1, &m_id); }

void GLBuffer::init() {
    glGenBuffers(1, &m_id);
    bind();
    glBufferData(m_type, m_size, m_data, m_io);
}

void GLBuffer::updateData(const void *data, size_t size, size_t offset) {
    if (size > m_size) {
        m_size = size;
        glNamedBufferData(m_id, size, data, m_io);
    } else {
        if (size == 0) {
            size = m_size;
        }
        glNamedBufferSubData(m_id, offset, size, data);
    }
}

void GLBuffer::bind() const { glBindBuffer(m_type, m_id); }

void GLBuffer::bindBase(uint32_t index) const {
    glBindBufferBase(m_type, index, m_id);
}

void GLBuffer::unbind() const { glBindBuffer(m_type, 0); }

GLVertexBuffer::GLVertexBuffer(const void *data, size_t size, BufferIOType io)
    : GLBuffer(GL_ARRAY_BUFFER, TRANSLATE(io), data, size) {}

GLIndexBuffer::GLIndexBuffer(const uint32_t *data, uint32_t count,
                             BufferIOType io)
    : GLBuffer(GL_ELEMENT_ARRAY_BUFFER, TRANSLATE(io), data,
               count * sizeof(uint32_t)),
      m_count(count) {}

uint32_t GLIndexBuffer::getCount() const { return m_count; }

uint32_t GLUniformBuffer::s_count = 0;

GLUniformBuffer::GLUniformBuffer(const void *data, size_t size, BufferIOType io)
    : GLBuffer(GL_UNIFORM_BUFFER, TRANSLATE(io), data, size) {
    m_base = s_count++;
}

uint32_t GLUniformBuffer::getBindingPoint() const { return m_base; }

void GLUniformBuffer::bind() const {
    GLBuffer::bind();
    GLBuffer::bindBase(m_base);
}

}  // namespace sd
