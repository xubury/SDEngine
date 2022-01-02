#ifndef SD_GL_FRAME_BUFFER_HPP
#define SD_GL_FRAME_BUFFER_HPP

#include "Graphics/Framebuffer.hpp"
#include <vector>
#include <GL/glew.h>

namespace SD {

class GLTexture;

class GLFramebuffer : public Framebuffer {
   public:
    GLFramebuffer(int width, int height);
    virtual ~GLFramebuffer();

    uint32_t GetId() const override { return m_id; }

    int GetWidth() const override { return m_width; }
    int GetHeight() const override { return m_height; }

    void Attach(const TextureSpec &spec) override;

    void Attach(const RenderbufferSpec &renderbuffer) override;

    void ReadPixels(uint32_t attachment_id, int level, int x, int y, int z,
                    int w, int h, int d, size_t size,
                    void *data) const override;

    void ClearDepth(const float depth) override;

    void ClearAttachment(uint32_t attachment_id, const int *value) override;
    void ClearAttachment(uint32_t attachment_id,
                         const uint32_t *value) override;
    void ClearAttachment(uint32_t attachment_id, const float *value) override;

    Texture *GetTexture(uint32_t attachment_id) override;
    const Texture *GetTexture(uint32_t attachment_id) const override;

    const std::vector<GLenum> &GetDrawables() const { return m_drawables; }

   private:
    GLuint m_id;
    int m_width;
    int m_height;
    uint32_t m_texture_cnt;
    std::vector<Ref<void>> m_attachments;
    std::vector<GLenum> m_drawables;
};

}  // namespace SD

#endif /* SD_GL_FRAME_BUFFER_HPP */
