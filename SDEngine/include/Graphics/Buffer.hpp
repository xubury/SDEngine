#ifndef SD_BUFFER_HPP
#define SD_BUFFER_HPP

#include "Utility/Base.hpp"
#include "Graphics/Graphics.hpp"

namespace SD {

class SD_GRAPHICS_API Buffer {
   public:
    virtual ~Buffer() = default;

    Buffer(const Buffer &) = delete;

    Buffer &operator=(const Buffer &) = delete;

    virtual void UpdateData(const void *data, size_t size,
                            size_t offset = 0) = 0;

    virtual void Bind() const = 0;

    virtual void BindBase(uint32_t index) const = 0;

    virtual void Unbind() const = 0;

    virtual uint32_t GetId() const = 0;

   protected:
    Buffer() = default;
};

class SD_GRAPHICS_API VertexBuffer : virtual public Buffer {
   public:
    static Ref<VertexBuffer> Create(const void *data, size_t size,
                                    BufferIOType io);

    virtual ~VertexBuffer() = default;

   protected:
    VertexBuffer() = default;
};

class SD_GRAPHICS_API IndexBuffer : virtual public Buffer {
   public:
    static Ref<IndexBuffer> Create(const uint32_t *data, uint32_t count,
                                   BufferIOType io);

    virtual ~IndexBuffer() = default;

    uint32_t GetCount() const;

   protected:
    IndexBuffer(uint32_t count);

    uint32_t m_count;
};

class SD_GRAPHICS_API UniformBuffer : virtual public Buffer {
   public:
    static Ref<UniformBuffer> Create(const void *data, size_t size,
                                     BufferIOType io);

    virtual ~UniformBuffer() = default;

    virtual uint32_t GetBindingPoint() const = 0;

   protected:
    UniformBuffer() = default;
};

}  // namespace SD

#endif /* SD_BUFFER_HPP */
