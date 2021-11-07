#include "Renderer/Renderer.hpp"
#include "Renderer/RenderEngine.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Device.hpp"
#include "Utility/Log.hpp"

namespace sd {

struct QuadVertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 texCoord;
    float texIndex;
    QuadVertex() : position(0.f), color(0.f), texCoord(0.f), texIndex(0.f) {}
};

struct Quad {
    std::array<QuadVertex, 4> vertices;
    glm::vec3 getCenter() const {
        return (vertices[0].position + vertices[1].position +
                vertices[2].position + vertices[3].position) /
               glm::vec3(4);
    }
};

struct RendererData {
    static const uint32_t MAX_QUADS = 20000;
    static const uint32_t MAX_VERTICES = MAX_QUADS * 4;
    static const uint32_t MAX_INDICES = MAX_QUADS * 6;
    static const uint32_t MAX_TEXTURE_SLOTS = 32;

    Ref<VertexArray> quadVAO;
    Ref<VertexBuffer> quadVBO;
    Ref<IndexBuffer> quadEBO;
    Ref<Shader> shader;

    size_t quadIndexCnt = 0;
    std::array<Quad, MAX_QUADS> quadVertexBufferBase;
    Quad* quadVertexBufferPtr = nullptr;

    std::array<glm::vec4, 4> quadVertexPositions;
    std::array<glm::vec2, 4> quadTexCoords;

    uint32_t textureSlotIndex = 1;
    std::array<Ref<Texture>, MAX_TEXTURE_SLOTS> textureSlots;

    glm::vec2 textOrigin;
    glm::vec2 textCursor;
};

static RendererData s_data;

void Renderer::init() {
    SD_CORE_TRACE("Initializing Renderer...");

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
    s_data.quadEBO = IndexBuffer::create(quadIndices.data(), quadIndices.size(),
                                         BufferIOType::STATIC);

    s_data.quadVBO =
        VertexBuffer::create(s_data.quadVertexBufferBase.data(),
                             s_data.quadVertexBufferBase.size() * sizeof(Quad),
                             BufferIOType::DYNAMIC);

    VertexBufferLayout layout;
    layout.push(BufferDataType::FLOAT, 3);  // position
    layout.push(BufferDataType::FLOAT, 4);  // color
    layout.push(BufferDataType::FLOAT, 2);  // texCoord
    layout.push(BufferDataType::FLOAT, 1);  // texIndex

    s_data.quadVAO = VertexArray::create();
    s_data.quadVAO->addVertexBuffer(s_data.quadVBO, layout);
    s_data.quadVAO->setIndexBuffer(s_data.quadEBO);

    s_data.quadVertexPositions[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
    s_data.quadVertexPositions[1] = {0.5f, -0.5f, 0.0f, 1.0f};
    s_data.quadVertexPositions[2] = {0.5f, 0.5f, 0.0f, 1.0f};
    s_data.quadVertexPositions[3] = {-0.5f, 0.5f, 0.0f, 1.0f};

    s_data.quadTexCoords[0] = {0.0f, 1.0f};
    s_data.quadTexCoords[1] = {1.0f, 1.0f};
    s_data.quadTexCoords[2] = {1.0f, 0.0f};
    s_data.quadTexCoords[3] = {0.0f, 0.0f};

    const float color[4] = {1, 1, 1, 1};
    s_data.textureSlots[0] = Texture::create(
        1, 1, 1, TextureType::TEX_2D, TextureFormat::RGBA,
        TextureFormatType::FLOAT, TextureWrap::REPEAT, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR, &color);

    s_data.shader = Asset::manager().load<Shader>("shaders/2d.glsl");
}

void Renderer::beginScene(Camera& camera) {
    RenderEngine::updateShader(*s_data.shader, camera);
    s_data.shader->bind();
    startBatch();
}

void Renderer::endScene() { flush(); }

void Renderer::flush() {
    if (s_data.quadIndexCnt == 0) {
        return;
    }
    size_t offset =
        s_data.quadVertexBufferPtr - s_data.quadVertexBufferBase.data();
    glm::vec3 viewPos = RenderEngine::getCamera()->getWorldPosition();

    // sort the drawing order to render transparent texture properly.
    std::sort(s_data.quadVertexBufferBase.begin(),
              s_data.quadVertexBufferBase.begin() + offset,
              [&viewPos](const Quad& lhs, const Quad& rhs) {
                  return glm::distance2(lhs.getCenter(), viewPos) >
                         glm::distance2(rhs.getCenter(), viewPos);
              });

    s_data.quadVBO->updateData(s_data.quadVertexBufferBase.data(),
                               offset * sizeof(Quad));

    for (uint32_t i = 0; i < s_data.textureSlotIndex; ++i) {
        s_data.shader->setTexture(i, s_data.textureSlots[i].get());
    }

    Device::instance().submit(*s_data.quadVAO, MeshTopology::TRIANGLES,
                              s_data.quadIndexCnt, 0);

    setTextOrigin(0, 0);
}

void Renderer::startBatch() {
    s_data.quadIndexCnt = 0;
    s_data.textureSlotIndex = 1;
    s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase.data();
}

void Renderer::nextBatch() {
    flush();
    startBatch();
}

void Renderer::setTextOrigin(float x, float y) {
    s_data.textOrigin.x = x;
    s_data.textOrigin.y = y;
    s_data.textCursor.x = 0;
    s_data.textCursor.y = 0;
}

void Renderer::drawQuad(const glm::mat4& transform, const glm::vec4& color) {
    if (s_data.quadIndexCnt >= RendererData::MAX_INDICES) {
        nextBatch();
    }
    for (uint32_t i = 0; i < 4; ++i) {
        s_data.quadVertexBufferPtr->vertices[i].position =
            transform * s_data.quadVertexPositions[i];
        s_data.quadVertexBufferPtr->vertices[i].color = color;
        s_data.quadVertexBufferPtr->vertices[i].texCoord =
            s_data.quadTexCoords[i];
        s_data.quadVertexBufferPtr->vertices[i].texIndex = 0;
    }
    ++s_data.quadVertexBufferPtr;
    s_data.quadIndexCnt += 6;
}

void Renderer::drawTexture(const Ref<Texture>& texture,
                           const glm::mat4& transform, const glm::vec4& color) {
    if (s_data.quadIndexCnt >= RendererData::MAX_INDICES) {
        nextBatch();
    }

    uint32_t textureIndex = 0;
    for (uint32_t i = 1; i < s_data.textureSlotIndex; ++i) {
        if (*s_data.textureSlots[i] == *texture) {
            textureIndex = i;
            break;
        }
    }

    if (textureIndex == 0) {
        if (s_data.textureSlotIndex >= RendererData::MAX_TEXTURE_SLOTS) {
            nextBatch();
        }
        textureIndex = s_data.textureSlotIndex;
        s_data.textureSlots[s_data.textureSlotIndex++] = texture;
    }

    for (uint32_t i = 0; i < 4; ++i) {
        s_data.quadVertexBufferPtr->vertices[i].position =
            transform * s_data.quadVertexPositions[i];
        s_data.quadVertexBufferPtr->vertices[i].color = color;
        s_data.quadVertexBufferPtr->vertices[i].texCoord =
            s_data.quadTexCoords[i];
        s_data.quadVertexBufferPtr->vertices[i].texIndex = textureIndex;
    }
    ++s_data.quadVertexBufferPtr;
    s_data.quadIndexCnt += 6;
}

void Renderer::drawBillboard(const Ref<Texture>& texture, const glm::vec3& pos,
                             const glm::vec2& scale, const glm::vec4& color) {
    drawTexture(
        texture,
        glm::translate(glm::mat4(1.0f), pos) *
            glm::toMat4(RenderEngine::getCamera()->getWorldRotation()) *
            glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f)),
        color);
}

void Renderer::drawText(Font& font, const std::wstring& text, int pixelSize,
                        const glm::mat4& transform, const glm::vec4& color) {
    glm::mat4 t =
        glm::translate(glm::mat4(1.0f),
                       glm::vec3(s_data.textOrigin.x, s_data.textOrigin.y, 0)) *
        transform;
    for (const auto c : text) {
        if (c == '\n') {
            s_data.textCursor.x = 0;
            s_data.textCursor.y -= pixelSize;
            continue;
        }
        const Character& ch = font.getCharacter(c, pixelSize);
        glm::mat4 offset =
            glm::translate(
                glm::mat4(1.0f),
                glm::vec3(s_data.textCursor.x + ch.bearing.x + ch.size.x * 0.5f,
                          s_data.textCursor.y + ch.bearing.y - ch.size.y * 0.5f,
                          0)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(ch.size.x, ch.size.y, 1.0f));
        drawTexture(ch.texture, t * offset, color);
        s_data.textCursor.x += ch.advance >> 6;
    }
}

void Renderer::drawMesh(const Mesh& mesh) {
    Device::instance().setPolygonMode(
        mesh.isWireframe() ? PolygonMode::LINE : PolygonMode::FILL, Face::BOTH);
    VertexArray* vao = mesh.getVertexArray();
    SD_CORE_ASSERT(vao, "Invalid mesh!");
    Device::instance().submit(*vao, mesh.getTopology(),
                              vao->getIndexBuffer()->getCount(), 0);
}

}  // namespace sd
