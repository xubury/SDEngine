#ifndef SD_BUFFER_HPP
#define SD_BUFFER_HPP

#include "Core/Export.hpp"
#include <stdint.h>
#include <GL/gl.h>

namespace sd {

class SD_API GLBuffer {
   public:
    GLBuffer(const GLBuffer &) = delete;

    GLBuffer &operator=(const GLBuffer &) = delete;

    void updateData(const void *data, size_t size, size_t offset = 0);

    void bind() const;

    void bindBase(GLuint index) const;

    void unbind() const;

    GLuint id() const;

   protected:
    GLBuffer(GLenum type, GLenum io, const void *data, size_t size);

    virtual ~GLBuffer();

   private:
    GLuint m_id;

    GLenum m_type;
    GLenum m_io;

    const void *m_data;
    size_t m_size;
};

class SD_API VertexBuffer : public GLBuffer {
   public:
    VertexBuffer(const void *data, size_t size, GLenum io);

    ~VertexBuffer() = default;
};

class SD_API IndexBuffer : public GLBuffer {
   public:
    IndexBuffer(const uint32_t *data, uint32_t count, GLenum io);

    ~IndexBuffer() = default;

    uint32_t getCount() const;

   private:
    uint32_t m_count;
};

class SD_API UniformBuffer : public GLBuffer {
   public:
    UniformBuffer(const void *data, size_t size, GLenum io);

    ~UniformBuffer() = default;

    void bind() const;

    uint32_t getBindingPoint() const;

   private:
    uint32_t m_base;
    static uint32_t s_count;
};

}  // namespace sd

#endif
