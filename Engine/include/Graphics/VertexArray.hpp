#ifndef SD_VERTEX_ARRAY_HPP
#define SD_VERTEX_ARRAY_HPP

#include "Core/Base.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/BufferLayout.hpp"

namespace sd {

class SD_API VertexArray {
   public:
    static Ref<VertexArray> create();

    VertexArray();

    ~VertexArray();

    VertexArray(const VertexArray &) = delete;

    VertexArray &operator=(const VertexArray &) = delete;

    void bind() const;

    void unbind() const;

    void addVertexBuffer(const Ref<VertexBuffer> &buffer,
                         const VertexBufferLayout &layout, int index = -1);

    void setIndexBuffer(const Ref<IndexBuffer> &buffer);
    void updateBuffer(size_t index, const void *data, size_t size,
                      size_t offset = 0);

   private:
    uint32_t m_id;
    std::vector<Ref<VertexBuffer>> m_vertexBuffers;
    std::vector<VertexBufferLayout> m_layouts;

    Ref<IndexBuffer> m_indexBuffer;
};

}  // namespace sd

#endif /* SD_VERTEX_ARRAY_HPP */
