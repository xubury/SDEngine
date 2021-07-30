#include "GL/glew.h"
#include "Graphics/Buffer.hpp"

namespace sd {

GLBuffer::GLBuffer(GLenum type, GLenum io, const void *data, size_t size)
    : m_type(type), m_io(io), m_data(data), m_size(size) {
    glGenBuffers(1, &m_id);
    bind();
    glBufferData(m_type, m_size, m_data, m_io);
}

GLBuffer::~GLBuffer() { glDeleteBuffers(1, &m_id); }

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

void GLBuffer::bindBase(GLuint index) const {
    glBindBufferBase(m_type, index, m_id);
}

void GLBuffer::unbind() const { glBindBuffer(m_type, 0); }

GLuint GLBuffer::id() const { return m_id; }

Ref<VertexBuffer> VertexBuffer::create(const void *data, size_t size,
                                       GLenum io) {
    return createRef<VertexBuffer>(data, size, io);
}

VertexBuffer::VertexBuffer(const void *data, size_t size, GLenum io)
    : GLBuffer(GL_ARRAY_BUFFER, io, data, size) {}

Ref<IndexBuffer> IndexBuffer::create(const uint32_t *data, uint32_t count,
                                     GLenum io) {
    return createRef<IndexBuffer>(data, count, io);
}

IndexBuffer::IndexBuffer(const uint32_t *data, uint32_t count, GLenum io)
    : GLBuffer(GL_ELEMENT_ARRAY_BUFFER, io, data, count * sizeof(uint32_t)),
      m_count(count) {}

uint32_t IndexBuffer::getCount() const { return m_count; }

uint32_t UniformBuffer::s_count = 0;

Ref<UniformBuffer> UniformBuffer::create(const void *data, size_t size,
                                         GLenum io) {
    return createRef<UniformBuffer>(data, size, io);
}

UniformBuffer::UniformBuffer(const void *data, size_t size, GLenum io)
    : GLBuffer(GL_UNIFORM_BUFFER, io, data, size) {
    m_base = s_count++;
}

uint32_t UniformBuffer::getBindingPoint() const { return m_base; }

void UniformBuffer::bind() const {
    GLBuffer::bind();
    GLBuffer::bindBase(m_base);
}

}  // namespace sd
