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

    virtual void Resize(int width, int height) override;
    virtual void Validate() override;

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

    void Clear();

   private:
    std::vector<Ref<void>> m_attachments;
    std::vector<GLenum> m_drawables;
};

}  // namespace SD

#endif /* SD_GL_FRAME_BUFFER_HPP */
