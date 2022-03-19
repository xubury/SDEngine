#include "Graphics/CascadeShadow.hpp"

namespace SD {

const int shadow_map_width = 4086;
const int shadow_map_height = 4086;

CascadeShadow::CascadeShadow() : m_cascade_planes{1, 100, 500, 1000} {}

void CascadeShadow::CreateShadowMap()
{
    const float color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    m_cascade_map = Framebuffer::Create(
        {shadow_map_width,
         shadow_map_height,
         (int)m_cascade_planes.size(),
         {AttachmentDescription{AttachmentType::Array, DataFormat::Depth24,
                                MultiSampleLevel::X1}}});
    m_cascade_map->GetTexture()->SetWrap(TextureWrap::Border);
    m_cascade_map->GetTexture()->SetBorderColor(&color);
}

void CascadeShadow::DestroyShadowMap() { m_cascade_map.reset(); }

void CascadeShadow::SetNumOfCascades(int32_t num_of_cascades)
{
    const float color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    m_cascade_planes.resize(num_of_cascades);
    m_cascade_map->Resize(shadow_map_width, shadow_map_height,
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

void CascadeShadow::ComputeCascadeLightMatrix(const Transform &transform,
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
