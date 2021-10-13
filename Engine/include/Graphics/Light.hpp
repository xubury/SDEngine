#ifndef SD_LIGHT_HPP
#define SD_LIGHT_HPP

#include "Utility/Export.hpp"
#include "Utility/Transform.hpp"
#include "Graphics/RenderTarget.hpp"
#include "Graphics/Camera.hpp"
#include <glm/glm.hpp>

namespace sd {

class SD_API Light {
   public:
    glm::vec3 ambient = glm::vec3(1.0f);
    glm::vec3 diffuse = glm::vec3(1.0f);
    glm::vec3 specular = glm::vec3(1.0f);

    float cutOff = 25.f;
    float outerCutOff = 35.f;
    float constant = 1.0f;
    float linear = 0.09;
    float quadratic = 0.032f;

    bool isDirectional = false;

   public:
    Light();

    void setCastShadow(bool cast);
    bool isCastShadow() const;

    void createShadowMap();
    void destroyShadowMap();

    const glm::mat4 &getProjectionView() const { return m_projectionView; }

    void computeLightSpaceMatrix(const Transform &transform,
                                 const Camera *camera);

    const RenderTarget &getRenderTarget() const { return m_target; };

    Texture *getShadowMap() const;

    template <typename Archive>
    void serialize(Archive &archive) {
        archive(ambient, diffuse, specular, cutOff, outerCutOff, constant,
                linear, quadratic, isDirectional, m_isCastShadow);
    }

   private:
    static void computeBoundingBox(const Transform &transform,
                                   const Camera &camera, glm::vec3 &min,
                                   glm::vec3 &max);
    RenderTarget m_target;
    glm::mat4 m_projectionView;

    bool m_isCastShadow;
};

}  // namespace sd

#endif /* SD_LIGHT_HPP */
