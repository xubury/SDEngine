#ifndef SD_SHADOW_MAP_HPP
#define SD_SHADOW_MAP_HPP

#include "Utils/Export.hpp"
#include "Utils/Transform.hpp"
#include "Graphics/RenderTarget.hpp"
#include "Graphics/Camera.hpp"

namespace sd {

class SD_API ShadowMap {
   public:
    ShadowMap();

    const glm::mat4 &getProjectionView() const { return m_projectionView; }

    void computeLightSpaceMatrix(const Transform &transform,
                                 const Camera *camera);

    const RenderTarget &getRenderTarget() const { return m_target; };

    Texture *getShadowMap() const;

   private:
    static void computeBoundingBox(const Transform &transform,
                                   const PerspectiveCamera &camera,
                                   glm::vec3 &min, glm::vec3 &max);
    RenderTarget m_target;
    glm::mat4 m_projectionView;
};

}  // namespace sd

#endif /* SD_SHADOW_MAP_HPP */
