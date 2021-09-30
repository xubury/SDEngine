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

    void setTransfrom(Transform *transform);

    void computeLightSpaceMatrix(const Camera *camera);

    const RenderTarget &getRenderTarget() const { return m_target; };

    const glm::mat4 &getProjectionView() const { return m_projectionView; }

    Texture *getShadowMap() const;
   private:
    void computeBoundingBox(const PerspectiveCamera &camera, glm::vec3 &min,
                            glm::vec3 &max);
    Transform *m_transform;
    RenderTarget m_target;
    glm::mat4 m_projectionView;
};

}  // namespace sd

#endif /* SD_SHADOW_MAP_HPP */
