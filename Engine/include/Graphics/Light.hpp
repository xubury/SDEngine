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
    Light();

    void setCastShadow(bool cast);
    bool isCastShadow() const;

    void setDirectional(bool directional);
    bool isDirectional() const;

    void setAmbient(const glm::vec3 &ambient);
    const glm::vec3 &getAmbient() const;

    void setDiffuse(const glm::vec3 &diffuse);
    const glm::vec3 &getDiffuse() const;

    void setSpecular(const glm::vec3 &specular);
    const glm::vec3 &getSpecular() const;

    void setCutOff(float cutOff);
    float getCutOff() const;

    void setOuterCutOff(float outerCutOff);
    float getOuterCutOff() const;

    void setConstant(float constant);
    float getConstant() const;

    void setLinear(float linear);
    float getLinear() const;

    void setQuadratic(float quadratic);
    float getQuadratic() const;

    void createShadowMap();
    void destroyShadowMap();

    const glm::mat4 &getProjectionView() const { return m_projectionView; }

    void computeLightSpaceMatrix(const Transform &transform,
                                 const Camera *camera);

    const RenderTarget &getRenderTarget() const { return m_target; };

    Texture *getShadowMap() const;

    template <typename Archive>
    void serialize(Archive &archive) {
        archive(m_ambient, m_diffuse, m_specular, m_cutOff, m_outerCutOff,
                m_constant, m_linear, m_quadratic, m_isDirectional,
                m_isCastShadow);
    }

   private:
    static void computeBoundingBox(const Transform &transform,
                                   const Camera &camera, glm::vec3 &min,
                                   glm::vec3 &max);
    RenderTarget m_target;
    glm::mat4 m_projectionView;

    bool m_isCastShadow;
    bool m_isDirectional;

    glm::vec3 m_ambient;
    glm::vec3 m_diffuse;
    glm::vec3 m_specular;

    float m_cutOff = 25.f;
    float m_outerCutOff = 35.f;
    float m_constant = 1.0f;
    float m_linear = 0.09;
    float m_quadratic = 0.032f;
};

}  // namespace sd

#endif /* SD_LIGHT_HPP */
