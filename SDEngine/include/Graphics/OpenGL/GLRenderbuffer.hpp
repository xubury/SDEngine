#ifndef GL_RENDERBUFFER_HPP
#define GL_RENDERBUFFER_HPP

#include "Graphics/Renderbuffer.hpp"
#include <GL/glew.h>

namespace SD {

class SD_GRAPHICS_API GLRenderbuffer : public Renderbuffer {
   public:
    GLRenderbuffer(int width, int height, int samples, DataFormat format,
                   DataFormatType format_type);

   private:
    void Allocate();
};

}  // namespace SD
#endif
