#ifndef SD_VERTEX_ARRAY_HPP
#define SD_VERTEX_ARRAY_HPP

#include "Utility/Base.hpp"
#include "Graphics/BufferLayout.hpp"
#include "Graphics/Buffer.hpp"

namespace SD {

class SD_API VertexArray {
   public:
    static Ref<VertexArray> create();

    virtual ~VertexArray() = default;

    VertexArray(const VertexArray &) = delete;

    VertexArray &operator=(const VertexArray &) = delete;

    virtual void bind() const = 0;

    virtual void addVertexBuffer(const Ref<VertexBuffer> &buffer,
                                 const VertexBufferLayout &layout,
                                 int index = -1) = 0;

    virtual void updateBuffer(size_t index, const void *data, size_t size,
                              size_t offset = 0) = 0;

    virtual void setIndexBuffer(const Ref<IndexBuffer> &buffer) = 0;
    virtual Ref<IndexBuffer> getIndexBuffer() = 0;

   protected:
    VertexArray() = default;
};

}  // namespace SD

#endif /* SD_VERTEX_ARRAY_HPP */
