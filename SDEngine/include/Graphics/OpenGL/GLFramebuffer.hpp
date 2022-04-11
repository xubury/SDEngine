#ifndef SD_GL_FRAME_BUFFER_HPP
#define SD_GL_FRAME_BUFFER_HPP

#include "Graphics/Framebuffer.hpp"
#include <vector>
#include <GL/glew.h>

namespace SD {

class GLFramebuffer : public Framebuffer {
   public:
    GLFramebuffer();
    virtual ~GLFramebuffer();

    uint32_t Handle() const override { return m_id; }

    void Attach(Texture &texture, int attachment, int level) override;
    void Attach(Renderbuffer &buffer, int attachment) override;
    Texture *GetAttachment(int attachment) override
    {
        return m_textures.at(attachment);
    }
    void Prepare() override;

    void ClearDepth(const float depth) override;

    void ClearAttachment(uint32_t attachment_id, const int *value) override;
    void ClearAttachment(uint32_t attachment_id,
                         const uint32_t *value) override;
    void ClearAttachment(uint32_t attachment_id, const float *value) override;

   private:
    int32_t m_width;
    int32_t m_height;
    GLuint m_id;
    std::vector<Texture *> m_textures;
    std::vector<GLenum> m_drawables;
};

}  // namespace SD

#endif /* SD_GL_FRAME_BUFFER_HPP */
