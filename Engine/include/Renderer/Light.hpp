#ifndef SD_LIGHT_HPP
#define SD_LIGHT_HPP

#include "Utility/Base.hpp"
#include "Utility/Serialize.hpp"
#include "Utility/Transform.hpp"
#include "Renderer/RenderTarget.hpp"
#include "Renderer/Camera.hpp"

namespace SD {

class SD_API Light {
   public:
    Light();

    void SetCastShadow(bool cast);
    bool IsCastShadow() const;

    void SetDirectional(bool directional);
    bool IsDirectional() const;

    void SetAmbient(const glm::vec3 &ambient);
    const glm::vec3 &GetAmbient() const;

    void SetDiffuse(const glm::vec3 &diffuse);
    const glm::vec3 &GetDiffuse() const;

    void SetSpecular(const glm::vec3 &specular);
    const glm::vec3 &GetSpecular() const;

    void SetCutOff(float cutOff);
    float GetCutOff() const;

    void SetOuterCutOff(float outerCutOff);
    float GetOuterCutOff() const;

    void SetConstant(float constant);
    float GetConstant() const;

    void SetLinear(float linear);
    float GetLinear() const;

    void SetQuadratic(float quadratic);
    float GetQuadratic() const;

    void CreateShadowMap();
    void DestroyShadowMap();

    const glm::mat4 &GetProjectionView() const { return m_projectionView; }

    void ComputeLightSpaceMatrix(const Transform &transform,
                                 const Camera *camera);

    Texture *GetShadowMap() const;

    RenderTarget &GetRenderTarget() { return m_shadowTarget; };

    SERIALIZE(m_ambient, m_diffuse, m_specular, m_cutOff, m_outerCutOff,
              m_constant, m_linear, m_quadratic, m_isDirectional,
              m_isCastShadow)

   private:
    static void ComputeBoundingBox(const Transform &transform,
                                   const Camera &camera, glm::vec3 &min,
                                   glm::vec3 &max);
    RenderTarget m_shadowTarget;
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

}  // namespace SD

#endif /* SD_LIGHT_HPP */
