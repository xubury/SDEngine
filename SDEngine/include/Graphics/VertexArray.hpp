#ifndef SD_VERTEX_ARRAY_HPP
#define SD_VERTEX_ARRAY_HPP

#include "Utility/Base.hpp"
#include "Graphics/BufferLayout.hpp"
#include "Graphics/Buffer.hpp"

namespace SD {

class SD_GRAPHICS_API VertexArray : public Resource {
   public:
    static Ref<VertexArray> Create();

    virtual ~VertexArray() = default;

    VertexArray(const VertexArray &) = delete;

    VertexArray &operator=(const VertexArray &) = delete;

    virtual void BindVertexBuffer(const VertexBuffer &buffer, int index) = 0;
    virtual void AddBufferLayout(const VertexBufferLayout &layout) = 0;

    virtual void BindIndexBuffer(const IndexBuffer &buffer) = 0;

   protected:
    VertexArray() = default;
};

}  // namespace SD

#endif /* SD_VERTEX_ARRAY_HPP */
