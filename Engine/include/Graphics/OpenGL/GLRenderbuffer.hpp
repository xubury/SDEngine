#ifndef GL_RENDERBUFFER_HPP
#define GL_RENDERBUFFER_HPP

#include "Graphics/Renderbuffer.hpp"
#include <GL/glew.h>

namespace SD {

class SD_GRAPHICS_API GLRenderbuffer : public Renderbuffer {
   public:
    GLRenderbuffer(int width, int height, int samples, DataFormat format,
                   DataFormatType format_type);

    uint32_t GetId() const override { return gl_id; }

   private:
    void Allocate();

    GLuint gl_id;
};

}  // namespace SD
#endif
