#ifndef SD_LIGHT_HPP
#define SD_LIGHT_HPP

#include "Utility/Base.hpp"
#include "Utility/Serialize.hpp"
#include "Utility/Transform.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Graphics/Camera.hpp"

namespace SD {

class SD_GRAPHICS_API Light {
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

    const std::vector<glm::mat4> &GetLevelProjectionView() const {
        return m_projection_views;
    }

    void SetCascadePlanes(const std::vector<float> &planes) {
        m_cascade_planes = planes;
    }

    void SetNumOfCascades(int32_t num_of_cascades) {
        m_cascade_planes.resize(num_of_cascades);
        if (m_is_cast_shadow) {
            // FIXME: just resize the framebuffer?
            DestroyShadowMap();
            CreateShadowMap();
        }
    }

    const std::vector<float> &GetCascadePlanes() const {
        return m_cascade_planes;
    }

    void ComputeCascadeLightMatrix(const Transform &transform,
                                   const Camera &camera);

    Framebuffer *GetCascadeMap() const { return m_cascade_map.get(); }

    SERIALIZE(m_ambient, m_diffuse, m_specular, m_cutoff, m_outer_cutoff,
              m_constant, m_linear, m_quadratic, m_is_directional,
              m_is_cast_shadow)

   private:
    Ref<Framebuffer> m_cascade_map;
    glm::mat4 m_projection_view;
    // cascade light pv
    std::vector<glm::mat4> m_projection_views;
    std::vector<float> m_cascade_planes;

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
