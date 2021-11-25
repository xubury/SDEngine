#ifndef SD_GL_FRAME_BUFFER_HPP
#define SD_GL_FRAME_BUFFER_HPP

#include "Graphics/Framebuffer.hpp"
#include <vector>
#include <GL/glew.h>

namespace SD {

class GLTexture;

class GLFramebuffer : public Framebuffer {
   public:
    GLFramebuffer();
    virtual ~GLFramebuffer();

    uint32_t GetId() const override { return m_id; }

    bool AttachTexture(const Ref<Texture> &texture) override;

    void Clear() override;

    void SetDrawable(const std::vector<uint32_t> &colorAttachments) override;

    void ReadPixels(uint32_t attachmentId, int level, int x, int y, int z,
                    int w, int h, int d, size_t size,
                    void *data) const override;

    void Bind() override;

    void ClearDepth(const float depth) override;

    void ClearAttachment(uint32_t attachmentId, const int *value) override;
    void ClearAttachment(uint32_t attachmentId, const uint32_t *value) override;
    void ClearAttachment(uint32_t attachmentId, const float *value) override;

    Texture *GetTexture(uint32_t attachmentId) override;

    void Resize(int width, int height) override;

   private:
    GLuint m_id;
    uint32_t m_textureCnt;
    std::vector<std::pair<GLenum, Ref<Texture>>> m_attachments;
};

}  // namespace SD

#endif /* SD_GL_FRAME_BUFFER_HPP */
