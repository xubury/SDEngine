#ifndef SD_GL_VERTEX_ARRAY_HPP
#define SD_GL_VERTEX_ARRAY_HPP

#include "Utility/Base.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/BufferLayout.hpp"
#include "Graphics/OpenGL/GLBuffer.hpp"

namespace SD {

class SD_API GLVertexArray : public VertexArray {
   public:
    GLVertexArray();

    ~GLVertexArray();

    void Bind() const override;

    void AddVertexBuffer(const Ref<VertexBuffer> &buffer,
                         const VertexBufferLayout &layout,
                         int index = -1) override;

    void UpdateBuffer(size_t index, const void *data, size_t size,
                      size_t offset = 0) override;

    void SetIndexBuffer(const Ref<IndexBuffer> &buffer) override;

    Ref<IndexBuffer> GetIndexBuffer() override;

   private:
    GLuint m_id;
    std::vector<Ref<VertexBuffer>> m_vertexBuffers;
    Ref<IndexBuffer> m_indexBuffer;
    std::vector<VertexBufferLayout> m_layouts;
};

}  // namespace SD

#endif /* SD_VERTEX_ARRAY_HPP */
