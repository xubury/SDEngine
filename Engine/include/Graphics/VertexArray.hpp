#ifndef SD_VERTEX_ARRAY_HPP
#define SD_VERTEX_ARRAY_HPP

#include "Utils/Base.hpp"
#include "Graphics/BufferLayout.hpp"
#include "Graphics/Buffer.hpp"

namespace sd {

class SD_API VertexArray {
   public:
    static Ref<VertexArray> create();

    virtual ~VertexArray() = default;

    VertexArray(const VertexArray &) = delete;

    VertexArray &operator=(const VertexArray &) = delete;

    virtual void init() = 0;

    virtual void bind() const = 0;

    virtual void unbind() const = 0;

    virtual void addVertexBuffer(const Ref<VertexBuffer> &buffer,
                                 const VertexBufferLayout &layout,
                                 int index = -1) = 0;

    virtual void updateBuffer(size_t index, const void *data, size_t size,
                              size_t offset = 0) = 0;

    void setIndexBuffer(const Ref<IndexBuffer> &buffer);

    const Ref<IndexBuffer> getIndexBuffer() const;

   protected:
    VertexArray() = default;

    Ref<IndexBuffer> m_indexBuffer;
};

}  // namespace sd

#endif /* SD_VERTEX_ARRAY_HPP */
