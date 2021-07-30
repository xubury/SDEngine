#include "Graphics/Renderer2D.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Camera.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <stdint.h>

namespace sd {

struct QuadVertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 texCoord;
};

struct CameraData {
    glm::mat4 viewProjection;
};

struct Renderer2DData {
    static const uint32_t MAX_QUADS = 20000;
    static const uint32_t MAX_VERTICES = MAX_QUADS * 4;
    static const uint32_t MAX_INDICES = MAX_QUADS * 6;
    static const uint32_t maxTextureSlots = 32;

    Ref<VertexArray> quadVAO;
    Ref<VertexBuffer> quadVBO;
    Ref<IndexBuffer> quadEBO;
    Ref<Shader> shader;

    uint32_t quadIndexCnt = 0;
    QuadVertex* quadVertexBufferBase = nullptr;
    QuadVertex* quadVertexBufferPtr = nullptr;

    std::array<glm::vec4, 4> quadVertexPosition;

    CameraData cameraBuffer;
    Ref<UniformBuffer> cameraUBO;
};

static Renderer2DData s_data;

void Renderer2D::init() {
    s_data.quadVertexBufferBase = new QuadVertex[s_data.MAX_VERTICES];

    for (int i = 0; i < 4; ++i) {
        s_data.quadVertexBufferBase[i].position = s_data.quadVertexPosition[i];
    }

    std::array<uint32_t, s_data.MAX_INDICES> quadIndices;
    uint32_t offset = 0;
    for (uint32_t i = 0; i < s_data.MAX_INDICES; i += 6) {
        quadIndices[i + 0] = offset + 0;
        quadIndices[i + 1] = offset + 1;
        quadIndices[i + 2] = offset + 2;

        quadIndices[i + 3] = offset + 2;
        quadIndices[i + 4] = offset + 3;
        quadIndices[i + 5] = offset + 0;

        offset += 4;
    }
    s_data.quadEBO = IndexBuffer::create(quadIndices.data(), s_data.MAX_INDICES,
                                         GL_STATIC_DRAW);

    s_data.quadVBO = VertexBuffer::create(
        s_data.quadVertexBufferBase, s_data.MAX_VERTICES * sizeof(QuadVertex),
        GL_STATIC_DRAW);

    VertexBufferLayout layout;
    layout.push<float>(3);  // position
    layout.push<float>(4);  // color
    layout.push<float>(2);  // texCoord

    s_data.quadVAO = VertexArray::create();
    s_data.quadVAO->addVertexBuffer(s_data.quadVBO, layout);
    s_data.quadVAO->setIndexBuffer(s_data.quadEBO);

    s_data.quadVertexPosition[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
    s_data.quadVertexPosition[1] = {0.5f, -0.5f, 0.0f, 1.0f};
    s_data.quadVertexPosition[2] = {0.5f, 0.5f, 0.0f, 1.0f};
    s_data.quadVertexPosition[3] = {-0.5f, 0.5f, 0.0f, 1.0f};

    s_data.cameraUBO = UniformBuffer::create(
        &s_data.cameraBuffer, sizeof(CameraData), GL_STATIC_DRAW);

    s_data.shader = Shader::create();
    s_data.shader->loadFromFile("assets/shaders/simple.vert",
                                "assets/shaders/simple.frag");
}

void Renderer2D::destory() { delete s_data.quadVertexBufferBase; }

void Renderer2D::beginScene(const OrthographicCamera& camera) {
    s_data.cameraBuffer.viewProjection = camera.getViewPorjection();
    s_data.cameraUBO->updateData(&s_data.cameraBuffer, sizeof(CameraData));
    startBatch();
}

void Renderer2D::endScene() { flush(); }

void Renderer2D::flush() {
    if (s_data.quadIndexCnt == 0) {
        return;
    }

    size_t dataSize = (uint8_t*)s_data.quadVertexBufferPtr -
                      (uint8_t*)s_data.quadVertexBufferBase;
    s_data.quadVBO->updateData(s_data.quadVertexBufferBase, dataSize);

    s_data.shader->bind();
    s_data.shader->setUniformBuffer("Camera", *s_data.cameraUBO);

    s_data.quadVAO->bind();
    glDrawElements(GL_TRIANGLES, s_data.quadIndexCnt, GL_UNSIGNED_INT, 0);
}

void Renderer2D::startBatch() {
    s_data.quadIndexCnt = 0;
    s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;
}

void Renderer2D::nextBatch() {
    flush();
    startBatch();
}

void Renderer2D::drawQuad(const glm::mat4& transform, const glm::vec4& color) {
    if (s_data.quadIndexCnt >= Renderer2DData::MAX_INDICES) {
        nextBatch();
    }
    size_t size = s_data.quadVertexPosition.size();
    for (uint32_t i = 0; i < size; ++i) {
        s_data.quadVertexBufferPtr->position =
            transform * s_data.quadVertexPosition[i];
        s_data.quadVertexBufferPtr->color = color;
        ++s_data.quadVertexBufferPtr;
    }
    s_data.quadIndexCnt += 6;
}

}  // namespace sd
