#ifndef GL_RENDERBUFFER_HPP
#define GL_RENDERBUFFER_HPP

#include "Graphics/Renderbuffer.hpp"

namespace SD {

class SD_GRAPHICS_API GLRenderbuffer : public Renderbuffer {
   public:
    GLRenderbuffer(int width, int height, MultiSampleLevel samples,
                   DataFormat format);

   private:
    void Allocate();
};

}  // namespace SD
#endif
