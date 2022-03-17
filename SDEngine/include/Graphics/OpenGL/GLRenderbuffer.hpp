#ifndef GL_RENDERBUFFER_HPP
#define GL_RENDERBUFFER_HPP

#include "Graphics/Renderbuffer.hpp"
#include <GL/glew.h>

namespace SD {

class SD_GRAPHICS_API GLRenderbuffer : public Renderbuffer {
   public:
    GLRenderbuffer(int width, int height, MultiSampleLevel samples,
                   DataFormat format);

    uint32_t GetId() const override { return m_id; }
   private:
    void Allocate();
    GLuint m_id;
};

}  // namespace SD
#endif
