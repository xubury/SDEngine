#include "Graphics/Light.hpp"

namespace SD {

const int SHADOW_MAP_WIDTH = 8192;
const int SHADOW_MAP_HEIGHT = 8192;

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
    m_shadow_map = Framebuffer::Create(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
    m_shadow_map->Attach(
        TextureSpec(1, TextureType::TEX_2D, DataFormat::DEPTH,
                    DataFormatType::FLOAT16, TextureWrap::BORDER,
                    TextureMagFilter::NEAREST, TextureMinFilter::NEAREST));
    m_shadow_map->GetTexture()->SetBorderColor(&color);
}

void Light::DestroyShadowMap() { m_shadow_map.reset(); }

Framebuffer *Light::GetShadowMap() const { return m_shadow_map.get(); }

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
