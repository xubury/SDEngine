#include "Graphics/Light.hpp"

namespace SD {

Light::Light()
    : m_is_cast_shadow(false),
      m_is_directional(false),
      m_ambient(1.0f),
      m_diffuse(1.0f),
      m_specular(1.0f),
      m_cutoff(glm::radians(25.f)),
      m_outer_cutoff(glm::radians(35.f)),
      m_constant(1.0f),
      m_linear(0.09f),
      m_quadratic(0.032)
{
}

bool Light::IsCastShadow() const { return m_is_cast_shadow; }

void Light::SetCastShadow(bool cast) { m_is_cast_shadow = cast; }

void Light::SetDirectional(bool directional) { m_is_directional = directional; }

bool Light::IsDirectional() const { return m_is_directional; }

void Light::SetAmbient(const glm::vec3 &ambient) { m_ambient = ambient; }

const glm::vec3 &Light::GetAmbient() const { return m_ambient; }

void Light::SetDiffuse(const glm::vec3 &diffuse) { m_diffuse = diffuse; }

const glm::vec3 &Light::GetDiffuse() const { return m_diffuse; }

void Light::SetSpecular(const glm::vec3 &specular) { m_specular = specular; }

const glm::vec3 &Light::GetSpecular() const { return m_specular; }

void Light::SetCutoff(float cutoff) { m_cutoff = cutoff; }

float Light::GetCutoff() const { return m_cutoff; }

void Light::SetOuterCutoff(float outer_cutoff)
{
    m_outer_cutoff = outer_cutoff;
}

float Light::GetOuterCutoff() const { return m_outer_cutoff; }

void Light::SetConstant(float constant) { m_constant = constant; }

float Light::GetConstant() const { return m_constant; }

void Light::SetLinear(float linear) { m_linear = linear; }

float Light::GetLinear() const { return m_linear; }

void Light::SetQuadratic(float quadratic) { m_quadratic = quadratic; }

float Light::GetQuadratic() const { return m_quadratic; }

}  // namespace SD
