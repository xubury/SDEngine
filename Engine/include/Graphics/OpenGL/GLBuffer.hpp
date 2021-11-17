#ifndef SD_GL_BUFFER_HPP
#define SD_GL_BUFFER_HPP

#include "Utility/Export.hpp"
#include "Utility/Base.hpp"
#include "Graphics/Buffer.hpp"
#include <GL/glew.h>

namespace SD {

class SD_API GLBuffer : virtual public Buffer {
   public:
    void UpdateData(const void *data, size_t size, size_t offset) override;

    void Bind() const override;

    void BindBase(uint32_t index) const override;

    void Unbind() const override;

    GLuint GetId() const { return m_id; }

   protected:
    GLBuffer(GLenum type, GLenum io, const void *data, size_t size);

    virtual ~GLBuffer();

   protected:
    GLuint m_id;

    GLenum m_type;
    GLenum m_io;

    size_t m_size;
};

class SD_API GLVertexBuffer : public VertexBuffer, public GLBuffer {
   public:
    GLVertexBuffer(const void *data, size_t size, BufferIOType io);

    ~GLVertexBuffer() = default;
};

class SD_API GLIndexBuffer : public IndexBuffer, public GLBuffer {
   public:
    GLIndexBuffer(const uint32_t *data, uint32_t count, BufferIOType io);

    void UpdateData(const void *data, size_t size, size_t offset) override;

    ~GLIndexBuffer() = default;
};

class SD_API GLUniformBuffer : public UniformBuffer, public GLBuffer {
   public:
    GLUniformBuffer(const void *data, size_t size, BufferIOType io);

    ~GLUniformBuffer() = default;

    uint32_t GetBindingPoint() const override;

    void Bind() const override;

   private:
    uint32_t m_base;
    static uint32_t s_count;
};

}  // namespace SD

#endif /* SD_GL_BUFFER_HPP */
