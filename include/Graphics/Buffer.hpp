#ifndef SD_BUFFER_HPP
#define SD_BUFFER_HPP

#include "Core/Export.hpp"
#include "Graphics/BufferLayout.hpp"
#include <stdint.h>

namespace sd {

class SD_API VertexBuffer {
   public:
    VertexBuffer(const void *vertices, size_t size);

    ~VertexBuffer();

    VertexBuffer(const VertexBuffer &) = delete;

    VertexBuffer &operator=(const VertexBuffer &) = delete;

    void bind() const;

    void unbind() const;

    const BufferLayout &getLayout() const;

    void setLayout(const BufferLayout &layout);

    size_t getSize() const;

    void update(const void *vertices, size_t size);

   private:
    uint32_t m_bufferId;
    BufferLayout m_layout;
    size_t m_size;
};

class SD_API IndexBuffer {
   public:
    IndexBuffer(const uint32_t *indices, size_t count);

    ~IndexBuffer();

    IndexBuffer(const IndexBuffer &) = delete;

    IndexBuffer &operator=(const IndexBuffer &) = delete;

    void bind() const;

    void unbind() const;

    size_t getCount() const;

    void update(const uint32_t *indices, size_t count);

   private:
    uint32_t m_bufferId;
    size_t m_count;
};

class SD_API UniformBuffer {
   public:
    UniformBuffer(size_t size);

    ~UniformBuffer();

    UniformBuffer(const UniformBuffer &) = delete;

    UniformBuffer &operator=(const UniformBuffer &) = delete;

    void setData(const void *data, size_t offset, size_t size);

    void bind() const;

    void unbind() const;

    uint32_t getBindingPoint() const;

   private:
    uint32_t m_bufferId;
    size_t m_size;
    uint32_t m_slot;
    static uint32_t s_count;
};

}  // namespace sd

#endif
