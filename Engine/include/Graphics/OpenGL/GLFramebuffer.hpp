#ifndef SD_GL_FRAME_BUFFER_HPP
#define SD_GL_FRAME_BUFFER_HPP

#include "Graphics/Framebuffer.hpp"
#include <vector>
#include <GL/glew.h>

namespace sd {

class GLTexture;

class GLFramebuffer : public Framebuffer {
   public:
    GLFramebuffer();
    virtual ~GLFramebuffer();

    void init() override;
    bool attachTexture(Texture *texture) override;
    void setDrawable(const std::vector<uint32_t> &colorAttachments) override;

    int readPixels(uint32_t attachmentId, int x, int y) const override;

    void bind() const override;
    void unbind() const override;

    void copy(const Framebuffer *other, BufferBit bufferbit,
              TextureFilter filter) override;

   private:
    GLuint m_id;
    uint32_t m_textureCnt;
    std::vector<std::pair<GLenum, GLTexture *>> m_attachments;
};

}  // namespace sd

#endif /* SD_GL_FRAME_BUFFER_HPP */
