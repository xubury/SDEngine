#ifndef SD_SHADOW_MAP_HPP
#define SD_SHADOW_MAP_HPP

#include "Utils/Export.hpp"
#include "Utils/Transform.hpp"
#include "Graphics/RenderTarget.hpp"

namespace sd {

class SD_API ShadowMap {
   public:
    ShadowMap();

    void setTransfrom(Transform *transform);

   private:
    Transform *m_transform;
    RenderTarget m_target;
};

}  // namespace sd

#endif /* SD_SHADOW_MAP_HPP */
