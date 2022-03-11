#include "Graphics/Light.hpp"

namespace SD {

const int SHADOW_MAP_WIDTH = 8192;
const int SHADOW_MAP_HEIGHT = 8192;

Light::Light()
    : m_cascade_planes{1000.f / 50.f, 1000.f / 25.f, 1000.f / 10.f,
                       1000.f / 2.f, 1000.f},
      m_is_cast_shadow(false),
      m_is_directional(false),
      m_ambient(1.0f),
      m_diffuse(1.0f),
      m_specular(1.0f),
      m_cutoff(glm::radians(25.f)),
      m_outer_cutoff(glm::radians(35.f)),
      m_constant(1.0f),
      m_linear(0.09f),
      m_quadratic(0.032) {}

bool Light::IsCastShadow() const { return m_is_cast_shadow; }

void Light::SetCastShadow(bool cast) {
    m_is_cast_shadow = cast;
    if (cast) {
        CreateShadowMap();
    } else {
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

void Light::SetOuterCutoff(float outer_cutoff) {
    m_outer_cutoff = outer_cutoff;
}

float Light::GetOuterCutoff() const { return m_outer_cutoff; }

void Light::SetConstant(float constant) { m_constant = constant; }

float Light::GetConstant() const { return m_constant; }

void Light::SetLinear(float linear) { m_linear = linear; }

float Light::GetLinear() const { return m_linear; }

void Light::SetQuadratic(float quadratic) { m_quadratic = quadratic; }

float Light::GetQuadratic() const { return m_quadratic; }

void Light::CreateShadowMap() {
    const float color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    m_cascade_map = Framebuffer::Create();
    m_cascade_map->Attach(TextureSpec(
        SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 5, 1, TextureType::TEX_2D_ARRAY,
        DataFormat::DEPTH, DataFormatType::FLOAT16, TextureWrap::BORDER,
        TextureMagFilter::NEAREST, TextureMinFilter::NEAREST));
    m_cascade_map->Setup();
    m_cascade_map->GetTexture()->SetBorderColor(&color);
}

void Light::DestroyShadowMap() { m_cascade_map.reset(); }

static std::vector<glm::vec4> GetFrustumCorners(const glm::mat4 &project_view) {
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
                                     const glm::mat4 &projection_view) {
    glm::vec3 center(0);
    auto corners = GetFrustumCorners(projection_view);
    for (const auto &c : corners) {
        center += glm::vec3(c);
    }
    center /= corners.size();

    const auto light_view =
        glm::lookAt(center - transform.GetFront(), center, glm::vec3(0, 1, 0));

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
                                      const Camera &camera) {
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

void Light::ComputeLightSpaceMatrix(const Transform &transform,
                                    const Camera *camera) {
    glm::vec3 pos(0);
    glm::mat4 projection(1.f);
    if (m_is_directional) {
        glm::vec3 min;
        glm::vec3 max;
        ComputeBoundingBox(transform, *camera, min, max);
        // Add a offset for shadow behind the camera frustum
        min.z -= 300;
        glm::vec3 size = max - min;
        glm::vec3 center = (max + min) / 2.f;
        center.z -= size.z / 2.f;
        pos = transform.GetRotation() * center;
        projection = glm::ortho(-size.x / 2.f, size.x / 2.f, -size.y / 2.f,
                                size.y / 2.f, 0.f, size.z);
    } else {
        pos = transform.GetPosition();
        projection = glm::perspective(m_outer_cutoff * 2, 1.f, 1.f, 1000.f);
    }
    const glm::vec3 &up = transform.GetUp();
    const glm::vec3 &front = transform.GetFront();
    const glm::mat4 view = glm::lookAt(pos, pos + front, up);
    m_projection_view = projection * view;
}

void Light::ComputeBoundingBox(const Transform &transform, const Camera &camera,
                               glm::vec3 &min, glm::vec3 &max) {
    min = glm::vec3(std::numeric_limits<float>::max());
    max = glm::vec3(std::numeric_limits<float>::lowest());
    const glm::mat3 &rot_inv =
        glm::transpose(glm::mat3(transform.GetRotation()));
    const glm::mat3 &cam_to_light =
        rot_inv * glm::mat3(camera.GetWorldRotation());
    const float near_z = camera.GetNearZ();
    const float far_z = camera.GetFarZ();

    const float near_width = camera.GetNearWidth() / 2.f;
    const float far_width = camera.GetFarWidth() / 2.f;
    const float near_height = camera.GetNearHeight() / 2.f;
    const float far_height = camera.GetFarHeight() / 2.f;
    const glm::vec3 &cam_pos_in_light = rot_inv * camera.GetWorldPosition();
    std::array<glm::vec3, 8> vertices;
    vertices[0] = glm::vec3(-near_width, -near_height, -near_z);
    vertices[1] = glm::vec3(-near_width, near_height, -near_z);
    vertices[2] = glm::vec3(near_width, -near_height, -near_z);
    vertices[3] = glm::vec3(near_width, near_height, -near_z);

    vertices[4] = glm::vec3(-far_width, -far_height, -far_z);
    vertices[5] = glm::vec3(-far_width, far_height, -far_z);
    vertices[6] = glm::vec3(far_width, -far_height, -far_z);
    vertices[7] = glm::vec3(far_width, far_height, -far_z);

    for (const auto &vertex : vertices) {
        const glm::vec3 pos = cam_to_light * vertex + cam_pos_in_light;
        min = glm::min(min, pos);
        max = glm::max(max, pos);
    }
}

}  // namespace SD
