#ifndef SD_GL_VERTEX_ARRAY_HPP
#define SD_GL_VERTEX_ARRAY_HPP

#include "Utility/Base.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/BufferLayout.hpp"
#include "Graphics/OpenGL/GLBuffer.hpp"

namespace SD {

class SD_GRAPHICS_API GLVertexArray : public VertexArray {
   public:
    GLVertexArray();

    ~GLVertexArray();

    void BindVertexBuffer(const VertexBuffer &buffer, int32_t index) override;
    void AddBufferLayout(const VertexBufferLayout &layout) override;

    void BindIndexBuffer(const IndexBuffer &buffer) override;

    uint32_t Handle() const override { return m_id; }

   private:
    std::vector<VertexBufferLayout> m_layouts;

    uint32_t m_id;
    uint32_t m_attrib_id;
};

}  // namespace SD

#endif /* SD_VERTEX_ARRAY_HPP */
