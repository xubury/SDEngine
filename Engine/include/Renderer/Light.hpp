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

    void SetCutoff(float cutOff);
    float GetCutoff() const;

    void SetOuterCutoff(float outer_cutoff);
    float GetOuterCutoff() const;

    void SetConstant(float constant);
    float GetConstant() const;

    void SetLinear(float linear);
    float GetLinear() const;

    void SetQuadratic(float quadratic);
    float GetQuadratic() const;

    void CreateShadowMap();
    void DestroyShadowMap();

    const glm::mat4 &GetProjectionView() const { return m_projection_view; }

    void ComputeLightSpaceMatrix(const Transform &transform,
                                 const Camera *camera);

    Texture *GetShadowMap() const;

    RenderTarget &GetRenderTarget() { return m_shadow_target; };

    SERIALIZE(m_ambient, m_diffuse, m_specular, m_cutoff, m_outer_cutoff,
              m_constant, m_linear, m_quadratic, m_is_directional,
              m_is_cast_shadow)

   private:
    static void ComputeBoundingBox(const Transform &transform,
                                   const Camera &camera, glm::vec3 &min,
                                   glm::vec3 &max);
    RenderTarget m_shadow_target;
    glm::mat4 m_projection_view;

    bool m_is_cast_shadow;
    bool m_is_directional;

    glm::vec3 m_ambient;
    glm::vec3 m_diffuse;
    glm::vec3 m_specular;

    float m_cutoff;
    float m_outer_cutoff;
    float m_constant;
    float m_linear;
    float m_quadratic;
};

}  // namespace SD

#endif /* SD_LIGHT_HPP */
