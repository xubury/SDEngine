#include "Graphics/ShadowMap.hpp"
#include "Graphics/Texture.hpp"
#include "Utils/Assert.hpp"

namespace sd {

ShadowMap::ShadowMap() {
    const uint32_t width = 8192;
    const uint32_t height = 8192;

    m_target.addTexture(Texture::create(
        width, height, 1, TextureType::TEX_2D, TextureFormat::RGBA,
        TextureFormatType::UBYTE, TextureWrap::BORDER, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_NEAREST));
    auto shadowMap = Texture::create(
        width, height, 1, TextureType::TEX_2D, TextureFormat::DEPTH,
        TextureFormatType::FLOAT, TextureWrap::BORDER, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_NEAREST);
    const float color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    shadowMap->setBorderColor(&color);
    m_target.addTexture(shadowMap);

    m_target.init();
}

Texture *ShadowMap::getShadowMap() const { return m_target.getTexture(1); }

void ShadowMap::setTransfrom(Transform *transform) { m_transform = transform; }

void ShadowMap::computeLightSpaceMatrix(const Camera *camera) {
    glm::vec3 min;
    glm::vec3 max;
    auto perspectiveCam = dynamic_cast<const PerspectiveCamera *>(camera);
    if (perspectiveCam) {
        computeBoundingBox(*perspectiveCam, min, max);
    } else {
        SD_CORE_ASSERT(false, "OrthographicCamera not implemented!");
    }
    glm::vec3 size = max - min;
    glm::vec3 center = (max + min) / 2.f;
    center.z -= size.z / 2.f;
    glm::vec3 pos = m_transform->toWorldSpace(center);
    const glm::vec3 up = m_transform->getWorldUp();
    const glm::vec3 front = m_transform->getWorldFront();
    const glm::mat4 lightView = glm::lookAt(pos, pos + front, up);
    const glm::mat4 lightProjection = glm::ortho(
        -size.x / 2.f, size.x / 2.f, -size.y / 2.f, size.y / 2.f, 0.f, size.z);
    m_projectionView = lightProjection * lightView;
}

void ShadowMap::computeBoundingBox(const PerspectiveCamera &camera,
                                   glm::vec3 &min, glm::vec3 &max) {
    min = glm::vec3(std::numeric_limits<float>::max());
    max = glm::vec3(std::numeric_limits<float>::lowest());
    const glm::mat3 &rotation = m_transform->getWorldTransform();
    const glm::mat3 &rotaionInv = glm::transpose(rotation);
    const glm::mat3 &transform =
        rotaionInv * glm::mat3(camera.getWorldTransform());
    const float nearZ = 0;
    const float farZ = 1000.f;
    const float fov = camera.getFOV() / 2.f;
    const float aspect = camera.getAspect();
    const float nearHeight = nearZ * std::tan(glm::radians(fov));
    const float nearWidth = nearHeight * aspect;
    const float farHeight = farZ * std::tan(glm::radians(fov));
    const float farWidth = farHeight * aspect;
    const glm::vec3 &camPos = rotaionInv * camera.getWorldPosition();
    std::array<glm::vec3, 8> vertices;
    vertices[0] =
        transform * glm::vec4(-nearWidth, -nearHeight, -nearZ, 1) + camPos;
    vertices[1] =
        transform * glm::vec4(-nearWidth, nearHeight, -nearZ, 1) + camPos;
    vertices[2] =
        transform * glm::vec4(nearWidth, -nearHeight, -nearZ, 1) + camPos;
    vertices[3] =
        transform * glm::vec4(nearWidth, nearHeight, -nearZ, 1) + camPos;

    vertices[4] =
        transform * glm::vec4(-farWidth, -farHeight, -farZ, 1) + camPos;
    vertices[5] =
        transform * glm::vec4(-farWidth, farHeight, -farZ, 1) + camPos;
    vertices[6] =
        transform * glm::vec4(farWidth, -farHeight, -farZ, 1) + camPos;
    vertices[7] = transform * glm::vec4(farWidth, farHeight, -farZ, 1) + camPos;

    for (int i = 0; i < 8; ++i) {
        min = glm::min(min, vertices[i]);
        max = glm::max(max, vertices[i]);
    }
    // Add a offset for shadow behind the camera frustum
    min.z -= 10.f;
}

}  // namespace sd
