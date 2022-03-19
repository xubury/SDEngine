#include "Graphics/Light.hpp"

namespace SD {

const int SHADOW_MAP_WIDTH = 4086;
const int SHADOW_MAP_HEIGHT = 4086;

Light::Light()
    : m_cascade_planes{20.0f, 100.f, 500.f, 1000.f},
      m_is_cast_shadow(false),
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

void Light::SetCastShadow(bool cast)
{
    m_is_cast_shadow = cast;
    if (cast) {
        CreateShadowMap();
    }
    else {
        DestroyShadowMap();
    }
}

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

void Light::CreateShadowMap()
{
    const float color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    m_cascade_map = Framebuffer::Create(
        {SHADOW_MAP_WIDTH,
         SHADOW_MAP_HEIGHT,
         (int)m_cascade_planes.size(),
         {AttachmentDescription{AttachmentType::Array, DataFormat::Depth24,
                                MultiSampleLevel::X1}}});
    m_cascade_map->GetTexture()->SetWrap(TextureWrap::Border);
    m_cascade_map->GetTexture()->SetBorderColor(&color);
}

void Light::DestroyShadowMap() { m_cascade_map.reset(); }

void Light::SetNumOfCascades(int32_t num_of_cascades)
{
    const float color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    m_cascade_planes.resize(num_of_cascades);
    m_cascade_map->Resize(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT,
                          m_cascade_planes.size());
    m_cascade_map->GetTexture()->SetWrap(TextureWrap::Border);
    m_cascade_map->GetTexture()->SetBorderColor(&color);
}

static std::vector<glm::vec4> GetFrustumCorners(const glm::mat4 &project_view)
{
    const auto &inv_pv = glm::inverse(project_view);
    std::vector<glm::vec4> corners;
    for (int x = 0; x <= 1; ++x) {
        for (int y = 0; y <= 1; ++y) {
            for (int z = 0; z <= 1; ++z) {
                const auto pt =
                    inv_pv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f,
                                       2.0f * z - 1.0f, 1.0f);
                corners.push_back(pt / pt.w);
            }
        }
    }
    return corners;
}

static glm::mat4 GetLightSpaceMatrix(const Transform &transform,
                                     const glm::mat4 &projection_view)
{
    glm::vec3 center(0);
    auto corners = GetFrustumCorners(projection_view);
    for (const auto &c : corners) {
        center += glm::vec3(c);
    }
    center /= corners.size();

    const auto light_view =
        glm::lookAt(center - transform.GetFront(), center, transform.GetUp());

    float min_x = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::min();
    float min_y = std::numeric_limits<float>::max();
    float max_y = std::numeric_limits<float>::min();
    float min_z = std::numeric_limits<float>::max();
    float max_z = std::numeric_limits<float>::min();
    for (const auto &pt : corners) {
        const auto trf = light_view * pt;
        min_x = std::min(min_x, trf.x);
        max_x = std::max(max_x, trf.x);
        min_y = std::min(min_y, trf.y);
        max_y = std::max(max_y, trf.y);
        min_z = std::min(min_z, trf.z);
        max_z = std::max(max_z, trf.z);
    }
    const float z_mult = 10.0f;
    min_z = min_z < 0 ? min_z * z_mult : min_z / z_mult;
    max_z = max_z < 0 ? max_z / z_mult : max_z * z_mult;
    const auto &light_proj =
        glm::ortho(min_x, max_x, min_y, max_y, min_z, max_z);
    return light_proj * light_view;
}

void Light::ComputeCascadeLightMatrix(const Transform &transform,
                                      const Camera &camera)
{
    const float fov = camera.GetFOV();
    const float aspect = camera.GetNearWidth() / camera.GetNearHeight();
    const uint32_t size = m_cascade_planes.size();
    m_projection_views.resize(size);
    for (uint32_t i = 0; i < size; ++i) {
        const float near_plane =
            i == 0 ? camera.GetNearZ() : m_cascade_planes[i - 1];
        const float far_plane = m_cascade_planes[i];
        m_projection_views[i] = GetLightSpaceMatrix(
            transform, glm::perspective(fov, aspect, near_plane, far_plane) *
                           camera.GetView());
    }
}

}  // namespace SD
