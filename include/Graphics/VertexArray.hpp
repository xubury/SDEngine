#ifndef SD_VERTEX_ARRAY_HPP
#define SD_VERTEX_ARRAY_HPP

#include "Core/Base.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/BufferLayout.hpp"

namespace sd {

class SD_API VertexArray {
   public:
    VertexArray();

    ~VertexArray();

    VertexArray(const VertexArray &) = delete;

    VertexArray &operator=(const VertexArray &) = delete;

    void bind() const;

    void unbind() const;

    void addVertexBuffer(const Ref<VertexBuffer> &buffer,
                         const VertexBufferLayout &layout, int index = -1);

    void updateBuffer(size_t index, const void *data, size_t size,
                      size_t offset = 0);

   private:
    uint32_t m_id;
    uint32_t m_vertexBufferId;
    std::vector<Ref<VertexBuffer>> m_buffers;
    std::vector<VertexBufferLayout> m_layouts;
};

}  // namespace sd

#endif /* SD_VERTEX_ARRAY_HPP */
