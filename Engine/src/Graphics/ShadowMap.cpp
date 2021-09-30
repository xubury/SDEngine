#include "Graphics/ShadowMap.hpp"
#include "Graphics/Texture.hpp"
#include "Utils/Assert.hpp"

namespace sd {

ShadowMap::ShadowMap() {
    const uint32_t width = 8192;
    const uint32_t height = 8192;

    auto shadowMap = Texture::create(
        width, height, 1, TextureType::TEX_2D, TextureFormat::DEPTH,
        TextureFormatType::FLOAT, TextureWrap::BORDER, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR);
    const float color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    shadowMap->setBorderColor(&color);
    m_target.addTexture(shadowMap);

    m_target.init();
}

Texture *ShadowMap::getShadowMap() const { return m_target.getTexture(); }

void ShadowMap::computeLightSpaceMatrix(const Transform &transform,
                                        const Camera *camera) {
    glm::vec3 min;
    glm::vec3 max;
    auto perspectiveCam = dynamic_cast<const PerspectiveCamera *>(camera);
    if (perspectiveCam) {
        computeBoundingBox(transform, *perspectiveCam, min, max);
    } else {
        SD_CORE_ASSERT(false, "OrthographicCamera not implemented!");
    }
    // Add a offset for shadow behind the camera frustum
    min.z -= 10.f;
    glm::vec3 size = max - min;
    glm::vec3 center = (max + min) / 2.f;
    center.z -= size.z / 2.f;
    const glm::vec3 pos = transform.getWorldRotation() * center;
    const glm::vec3 &up = transform.getWorldUp();
    const glm::vec3 &front = transform.getWorldFront();
    const glm::mat4 lightView = glm::lookAt(pos, pos + front, up);
    const glm::mat4 lightProjection = glm::ortho(
        -size.x / 2.f, size.x / 2.f, -size.y / 2.f, size.y / 2.f, 0.f, size.z);
    m_projectionView = lightProjection * lightView;
}

void ShadowMap::computeBoundingBox(const Transform &transform,
                                   const PerspectiveCamera &camera,
                                   glm::vec3 &min, glm::vec3 &max) {
    min = glm::vec3(std::numeric_limits<float>::max());
    max = glm::vec3(std::numeric_limits<float>::lowest());
    const glm::mat3 &rotaionInv =
        glm::transpose(glm::mat3(transform.getWorldRotation()));
    const glm::mat3 &lightTrans =
        rotaionInv * glm::mat3(camera.getWorldRotation());
    const float nearZ = camera.getNearZ();
    const float farZ = camera.getFarZ();
    const float fov = camera.getFOV() / 2.f;
    const float aspect = camera.getAspect();
    const float nearHeight = nearZ * std::tan(fov);
    const float nearWidth = nearHeight * aspect;
    const float farHeight = farZ * std::tan(fov);
    const float farWidth = farHeight * aspect;
    const glm::vec3 &camPos = rotaionInv * camera.getWorldPosition();
    std::array<glm::vec3, 8> vertices;
    vertices[0] =
        lightTrans * glm::vec3(-nearWidth, -nearHeight, -nearZ) + camPos;
    vertices[1] =
        lightTrans * glm::vec3(-nearWidth, nearHeight, -nearZ) + camPos;
    vertices[2] =
        lightTrans * glm::vec3(nearWidth, -nearHeight, -nearZ) + camPos;
    vertices[3] =
        lightTrans * glm::vec3(nearWidth, nearHeight, -nearZ) + camPos;

    vertices[4] = lightTrans * glm::vec3(-farWidth, -farHeight, -farZ) + camPos;
    vertices[5] = lightTrans * glm::vec3(-farWidth, farHeight, -farZ) + camPos;
    vertices[6] = lightTrans * glm::vec3(farWidth, -farHeight, -farZ) + camPos;
    vertices[7] = lightTrans * glm::vec3(farWidth, farHeight, -farZ) + camPos;

    for (const auto vertex : vertices) {
        min = glm::min(min, vertex);
        max = glm::max(max, vertex);
    }
}

}  // namespace sd
