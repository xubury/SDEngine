#include "Renderer/Light.hpp"

namespace SD {

const int SHADOW_MAP_WIDTH = 8192;
const int SHADOW_MAP_HEIGHT = 8192;

Light::Light()
    : m_isCastShadow(false),
      m_isDirectional(false),
      m_ambient(1.0f),
      m_diffuse(1.0f),
      m_specular(1.0f),
      m_cutOff(glm::radians(25.f)),
      m_outerCutOff(glm::radians(35.f)),
      m_constant(1.0f),
      m_linear(0.09f),
      m_quadratic(0.032) {}

bool Light::IsCastShadow() const { return m_isCastShadow; }

void Light::SetCastShadow(bool cast) {
    m_isCastShadow = cast;
    DestroyShadowMap();
    if (cast) {
        CreateShadowMap();
    }
}

void Light::SetDirectional(bool directional) { m_isDirectional = directional; }

bool Light::IsDirectional() const { return m_isDirectional; }

void Light::SetAmbient(const glm::vec3 &ambient) { m_ambient = ambient; }

const glm::vec3 &Light::GetAmbient() const { return m_ambient; }

void Light::SetDiffuse(const glm::vec3 &diffuse) { m_diffuse = diffuse; }

const glm::vec3 &Light::GetDiffuse() const { return m_diffuse; }

void Light::SetSpecular(const glm::vec3 &specular) { m_specular = specular; }

const glm::vec3 &Light::GetSpecular() const { return m_specular; }

void Light::SetCutOff(float cutOff) { m_cutOff = cutOff; }

float Light::GetCutOff() const { return m_cutOff; }

void Light::SetOuterCutOff(float outerCutOff) { m_outerCutOff = outerCutOff; }

float Light::GetOuterCutOff() const { return m_outerCutOff; }

void Light::SetConstant(float constant) { m_constant = constant; }

float Light::GetConstant() const { return m_constant; }

void Light::SetLinear(float linear) { m_linear = linear; }

float Light::GetLinear() const { return m_linear; }

void Light::SetQuadratic(float quadratic) { m_quadratic = quadratic; }

float Light::GetQuadratic() const { return m_quadratic; }

void Light::CreateShadowMap() {
    auto shadowMap = Texture::Create(
        SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 1, TextureType::TEX_2D,
        TextureFormat::DEPTH, TextureFormatType::FLOAT, TextureWrap::BORDER,
        TextureFilter::NEAREST, TextureMipmapFilter::NEAREST);
    const float color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    shadowMap->SetBorderColor(&color);
    m_shadowTarget.AddTexture(shadowMap);
    m_shadowTarget.CreateFramebuffer();
}

void Light::DestroyShadowMap() { m_shadowTarget.Clear(); }

Texture *Light::GetShadowMap() const {
    return m_isCastShadow ? m_shadowTarget.GetTexture() : nullptr;
}

void Light::ComputeLightSpaceMatrix(const Transform &transform,
                                    const Camera *camera) {
    glm::vec3 pos(0);
    glm::mat4 lightProjection(1.f);
    if (m_isDirectional) {
        glm::vec3 min;
        glm::vec3 max;
        ComputeBoundingBox(transform, *camera, min, max);
        // Add a offset for shadow behind the camera frustum
        min.z -= 300;
        glm::vec3 size = max - min;
        glm::vec3 center = (max + min) / 2.f;
        center.z -= size.z / 2.f;
        pos = transform.GetWorldRotation() * center;
        lightProjection = glm::ortho(-size.x / 2.f, size.x / 2.f, -size.y / 2.f,
                                     size.y / 2.f, 0.f, size.z);
    } else {
        pos = transform.GetWorldPosition();
        lightProjection = glm::perspective(m_outerCutOff * 2, 1.f, 1.f, 1000.f);
    }
    const glm::vec3 &up = transform.GetWorldUp();
    const glm::vec3 &front = transform.GetWorldFront();
    const glm::mat4 lightView = glm::lookAt(pos, pos + front, up);
    m_projectionView = lightProjection * lightView;
}

void Light::ComputeBoundingBox(const Transform &transform, const Camera &camera,
                               glm::vec3 &min, glm::vec3 &max) {
    min = glm::vec3(std::numeric_limits<float>::max());
    max = glm::vec3(std::numeric_limits<float>::lowest());
    const glm::mat3 &rotaionInv =
        glm::transpose(glm::mat3(transform.GetWorldRotation()));
    const glm::mat3 &lightTrans =
        rotaionInv * glm::mat3(camera.GetWorldRotation());
    const float nearZ = camera.GetNearZ();
    const float farZ = camera.GetFarZ();

    const float nearWidth = camera.GetNearWidth() / 2.f;
    const float farWidth = camera.GetFarWidth() / 2.f;
    const float nearHeight = camera.GetNearHeight() / 2.f;
    const float farHeight = camera.GetFarHeight() / 2.f;
    const glm::vec3 &camPos = rotaionInv * camera.GetWorldPosition();
    std::array<glm::vec3, 8> vertices;
    vertices[0] = glm::vec3(-nearWidth, -nearHeight, -nearZ);
    vertices[1] = glm::vec3(-nearWidth, nearHeight, -nearZ);
    vertices[2] = glm::vec3(nearWidth, -nearHeight, -nearZ);
    vertices[3] = glm::vec3(nearWidth, nearHeight, -nearZ);

    vertices[4] = glm::vec3(-farWidth, -farHeight, -farZ);
    vertices[5] = glm::vec3(-farWidth, farHeight, -farZ);
    vertices[6] = glm::vec3(farWidth, -farHeight, -farZ);
    vertices[7] = glm::vec3(farWidth, farHeight, -farZ);

    for (const auto &vertex : vertices) {
        const glm::vec pos = lightTrans * vertex + camPos;
        min = glm::min(min, pos);
        max = glm::max(max, pos);
    }
}

}  // namespace SD
