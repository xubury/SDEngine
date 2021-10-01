#ifndef SD_GL_VERTEX_ARRAY_HPP
#define SD_GL_VERTEX_ARRAY_HPP

#include "Utility/Base.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/BufferLayout.hpp"
#include "Graphics/OpenGL/GLBuffer.hpp"

namespace sd {

class SD_API GLVertexArray : public VertexArray {
   public:
    GLVertexArray();

    ~GLVertexArray();

    void init() override;

    void bind() const override;

    void unbind() const override;

    void addVertexBuffer(const Ref<VertexBuffer> &buffer,
                         const VertexBufferLayout &layout,
                         int index = -1) override;

    void updateBuffer(size_t index, const void *data, size_t size,
                      size_t offset = 0) override;

   private:
    GLuint m_id;
    std::vector<Ref<VertexBuffer>> m_vertexBuffers;
    std::vector<VertexBufferLayout> m_layouts;

};

}  // namespace sd

#endif /* SD_VERTEX_ARRAY_HPP */
