#include "Graphics/Light.hpp"
#include "Utility/Log.hpp"
#include "Utility/Assert.hpp"

namespace sd {

const int SHADOW_MAP_WIDTH = 8192;
const int SHADOW_MAP_HEIGHT = 8192;

Light::Light() : m_isCastShadow(false) {}

bool Light::isCastShadow() const { return m_isCastShadow; }

void Light::setCastShadow(bool cast) {
    m_isCastShadow = cast;
    destroyShadowMap();
    if (cast) {
        createShadowMap();
    }
}

void Light::createShadowMap() {
    auto shadowMap = Texture::create(
        SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 1, TextureType::TEX_2D,
        TextureFormat::DEPTH, TextureFormatType::FLOAT, TextureWrap::BORDER,
        TextureFilter::NEAREST, TextureMipmapFilter::NEAREST);
    const float color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    shadowMap->setBorderColor(&color);
    m_target.addTexture(shadowMap);
    m_target.createFramebuffer();
}

void Light::destroyShadowMap() { m_target.clear(); }

Texture *Light::getShadowMap() const {
    return m_isCastShadow ? m_target.getTexture() : nullptr;
}

void Light::computeLightSpaceMatrix(const Transform &transform,
                                    const Camera *camera) {
    glm::vec3 min;
    glm::vec3 max;
    if (isDirectional) {
        computeBoundingBox(transform, *camera, min, max);
        // Add a offset for shadow behind the camera frustum
        min.z -= camera->getFarZ();
    } else {
        min = glm::vec3(-50);
        max = glm::vec3(50);
    }
    glm::vec3 size = max - min;
    glm::vec3 center = (max + min) / 2.f;
    center.z -= size.z / 2.f;
    const glm::vec3 pos = isDirectional ? transform.getWorldRotation() * center
                                        : transform.getWorldPosition();
    const glm::vec3 &up = transform.getWorldUp();
    const glm::vec3 &front = transform.getWorldFront();
    const glm::mat4 lightView = glm::lookAt(pos, pos + front, up);
    const glm::mat4 lightProjection = glm::ortho(
        -size.x / 2.f, size.x / 2.f, -size.y / 2.f, size.y / 2.f, 0.f, size.z);
    m_projectionView = lightProjection * lightView;
}

void Light::computeBoundingBox(const Transform &transform, const Camera &camera,
                               glm::vec3 &min, glm::vec3 &max) {
    min = glm::vec3(std::numeric_limits<float>::max());
    max = glm::vec3(std::numeric_limits<float>::lowest());
    const glm::mat3 &rotaionInv =
        glm::transpose(glm::mat3(transform.getWorldRotation()));
    const glm::mat3 &lightTrans =
        rotaionInv * glm::mat3(camera.getWorldRotation());
    const float nearZ = camera.getNearZ();
    const float farZ = camera.getFarZ();

    const float nearWidth = camera.getNearWidth() / 2.f;
    const float farWidth = camera.getFarWidth() / 2.f;
    const float nearHeight = camera.getNearHeight() / 2.f;
    const float farHeight = camera.getFarHeight() / 2.f;
    const glm::vec3 &camPos = rotaionInv * camera.getWorldPosition();
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

}  // namespace sd
