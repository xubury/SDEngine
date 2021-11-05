#ifndef SD_BUFFER_HPP
#define SD_BUFFER_HPP

#include "Utility/Export.hpp"
#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"

namespace sd {

class SD_API Buffer {
   public:
    virtual ~Buffer() = default;

    Buffer(const Buffer &) = delete;

    Buffer &operator=(const Buffer &) = delete;

    virtual void updateData(const void *data, size_t size,
                            size_t offset = 0) = 0;

    virtual void bind() const = 0;

    virtual void bindBase(uint32_t index) const = 0;

    virtual void unbind() const = 0;

   protected:
    Buffer() = default;
};

class SD_API VertexBuffer : virtual public Buffer {
   public:
    static Ref<VertexBuffer> create(const void *data, size_t size,
                                    BufferIOType io);

    virtual ~VertexBuffer() = default;

   protected:
    VertexBuffer() = default;
};

class SD_API IndexBuffer : virtual public Buffer {
   public:
    static Ref<IndexBuffer> create(const uint32_t *data, uint32_t count,
                                   BufferIOType io);

    virtual ~IndexBuffer() = default;

    uint32_t getCount() const;

   protected:
    IndexBuffer(uint32_t count);

    uint32_t m_count;
};

class SD_API UniformBuffer : virtual public Buffer {
   public:
    static Ref<UniformBuffer> create(const void *data, size_t size,
                                     BufferIOType io);

    virtual ~UniformBuffer() = default;

    virtual uint32_t getBindingPoint() const = 0;

   protected:
    UniformBuffer() = default;
};

}  // namespace sd

#endif /* SD_BUFFER_HPP */
