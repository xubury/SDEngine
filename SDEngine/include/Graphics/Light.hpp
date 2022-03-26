#ifndef SD_LIGHT_HPP
#define SD_LIGHT_HPP

#include "Graphics/Export.hpp"
#include "Utility/Base.hpp"
#include "Utility/Serialize.hpp"
#include "Utility/Math.hpp"

namespace SD {

class SD_GRAPHICS_API Light {
   public:
    Light();

    void SetCastShadow(bool cast);
    bool IsCastShadow() const;

    void SetDirectional(bool directional);
    bool IsDirectional() const;

    void SetAmbient(const Vector3f &ambient);
    const Vector3f &GetAmbient() const;

    void SetDiffuse(const Vector3f &diffuse);
    const Vector3f &GetDiffuse() const;

    void SetSpecular(const Vector3f &specular);
    const Vector3f &GetSpecular() const;

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

    SERIALIZE(m_ambient, m_diffuse, m_specular, m_cutoff, m_outer_cutoff,
              m_constant, m_linear, m_quadratic, m_is_directional,
              m_is_cast_shadow)

   private:
    bool m_is_cast_shadow;
    bool m_is_directional;

    Vector3f m_ambient;
    Vector3f m_diffuse;
    Vector3f m_specular;

    float m_cutoff;
    float m_outer_cutoff;
    float m_constant;
    float m_linear;
    float m_quadratic;
};

}  // namespace SD

#endif /* SD_LIGHT_HPP */
