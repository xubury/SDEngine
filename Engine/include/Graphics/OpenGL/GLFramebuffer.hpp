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

    uint32_t getId() const override { return m_id; }

    bool attachTexture(const Ref<Texture> &texture) override;

    void clear() override;

    void setDrawable(const std::vector<uint32_t> &colorAttachments) override;

    void readPixels(uint32_t attachmentId, int level, int x, int y, int z,
                    int w, int h, int d, size_t size,
                    void *data) const override;

    void bind() override;

    void clearDepth(const float depth) override;

    void clearAttachment(uint32_t attachmentId, const int *value) override;
    void clearAttachment(uint32_t attachmentId, const uint32_t *value) override;
    void clearAttachment(uint32_t attachmentId, const float *value) override;

    Texture *getTexture(uint32_t attachmentId) override;

    void resize(int width, int height) override;

   private:
    GLuint m_id;
    uint32_t m_textureCnt;
    std::vector<std::pair<GLenum, Ref<GLTexture>>> m_attachments;
};

}  // namespace SD

#endif /* SD_GL_FRAME_BUFFER_HPP */
