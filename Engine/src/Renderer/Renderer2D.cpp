#include "Renderer/Renderer2D.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Graphics/Font.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <cstdint>

namespace sd {

struct QuadVertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 texCoord;
    float texIndex;
    uint32_t grayScale;
    uint32_t entityIndex;
    QuadVertex()
        : position(0.f),
          color(0.f),
          texCoord(0.f),
          texIndex(0.f),
          grayScale(0),
          entityIndex(0) {}
};

struct Renderer2DData {
    static const uint32_t MAX_QUADS = 20000;
    static const uint32_t MAX_VERTICES = MAX_QUADS * 4;
    static const uint32_t MAX_INDICES = MAX_QUADS * 6;
    static const uint32_t MAX_TEXTURE_SLOTS = 32;

    Ref<VertexArray> quadVAO;
    Ref<VertexBuffer> quadVBO;
    Ref<IndexBuffer> quadEBO;
    Ref<Shader> shader;

    size_t quadIndexCnt = 0;
    std::array<QuadVertex, MAX_VERTICES> quadVertexBufferBase;
    QuadVertex* quadVertexBufferPtr = nullptr;

    std::array<glm::vec4, 4> quadVertexPositions;
    std::array<glm::vec2, 4> quadTexCoords;

    uint32_t textureSlotIndex = 1;
    std::array<Ref<Texture>, MAX_TEXTURE_SLOTS> textureSlots;

    glm::vec2 textOrigin;
    glm::vec2 textCursor;
};

static Renderer2DData s_data;

void Renderer2D::init() {
    SD_CORE_TRACE("Initializing Renderer2D...");
    for (int i = 0; i < 4; ++i) {
        s_data.quadVertexBufferBase[i].position = s_data.quadVertexPositions[i];
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
                                         BufferIOType::STATIC);

    s_data.quadVBO = VertexBuffer::create(
        s_data.quadVertexBufferBase.data(),
        s_data.MAX_VERTICES * sizeof(QuadVertex), BufferIOType::DYNAMIC);

    VertexBufferLayout layout;
    layout.push<float>(3);     // position
    layout.push<float>(4);     // color
    layout.push<float>(2);     // texCoord
    layout.push<float>(1);     // texIndex
    layout.push<uint32_t>(1);  // grayScale
    layout.push<uint32_t>(1);  // entityIndex

    s_data.quadVAO = VertexArray::create();
    s_data.quadVAO->addVertexBuffer(s_data.quadVBO, layout);
    s_data.quadVAO->setIndexBuffer(s_data.quadEBO);

    s_data.quadVertexPositions[0] = {0.f, 0.f, 0.0f, 1.0f};
    s_data.quadVertexPositions[1] = {1.0f, 0.f, 0.0f, 1.0f};
    s_data.quadVertexPositions[2] = {1.0f, 1.0f, 0.0f, 1.0f};
    s_data.quadVertexPositions[3] = {0.f, 1.0f, 0.0f, 1.0f};

    s_data.quadTexCoords[0] = {0.0f, 1.0f};
    s_data.quadTexCoords[1] = {1.0f, 1.0f};
    s_data.quadTexCoords[2] = {1.0f, 0.0f};
    s_data.quadTexCoords[3] = {0.0f, 0.0f};

    uint32_t color = 0xffffffff;
    s_data.textureSlots[0] = Texture::create(
        1, 1, 1, TextureType::TEX_2D, TextureFormat::RGBA,
        TextureFormatType::UBYTE, TextureWrap::REPEAT, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR, &color);

    s_data.shader = Asset::manager().load<Shader>("shaders/simple.glsl");
}

void Renderer2D::beginScene(Camera& camera) {
    Device::instance().clear(BufferBitMask::DEPTH_BUFFER_BIT);
    Renderer::setCamera(camera, *s_data.shader);
    Device::instance().disable(Operation::DEPTH_TEST);
    startBatch();
}

void Renderer2D::endScene() {
    flush();
    Device::instance().enable(Operation::DEPTH_TEST);
}

void Renderer2D::flush() {
    if (s_data.quadIndexCnt == 0) {
        return;
    }

    size_t dataSize = (uint8_t*)s_data.quadVertexBufferPtr -
                      (uint8_t*)s_data.quadVertexBufferBase.data();
    s_data.quadVBO->updateData(s_data.quadVertexBufferBase.data(), dataSize);

    s_data.shader->bind();
    for (uint32_t i = 0; i < s_data.textureSlotIndex; ++i) {
        s_data.shader->setTexture(i, s_data.textureSlots[i].get());
    }

    Renderer::submit(*s_data.quadVAO, MeshTopology::TRIANGLES,
                     s_data.quadIndexCnt, 0);
    setTextOrigin(0, 0);
}

void Renderer2D::startBatch() {
    s_data.quadIndexCnt = 0;
    s_data.textureSlotIndex = 1;
    s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase.data();
}

void Renderer2D::nextBatch() {
    flush();
    startBatch();
}

void Renderer2D::drawQuad(const glm::mat4& transform, const glm::vec4& color) {
    if (s_data.quadIndexCnt >= Renderer2DData::MAX_INDICES) {
        nextBatch();
    }
    for (uint32_t i = 0; i < 4; ++i) {
        s_data.quadVertexBufferPtr->position =
            transform * s_data.quadVertexPositions[i];
        s_data.quadVertexBufferPtr->color = color;
        s_data.quadVertexBufferPtr->texCoord = s_data.quadTexCoords[i];
        s_data.quadVertexBufferPtr->texIndex = 0;
        ++s_data.quadVertexBufferPtr;
    }
    s_data.quadIndexCnt += 6;
}

void Renderer2D::drawTexture(const Ref<Texture>& texture,
                             const glm::mat4& transform,
                             const glm::vec4& color) {
    if (s_data.quadIndexCnt >= Renderer2DData::MAX_INDICES) {
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
        if (s_data.textureSlotIndex >= Renderer2DData::MAX_TEXTURE_SLOTS) {
            nextBatch();
        }
        textureIndex = s_data.textureSlotIndex;
        s_data.textureSlots[s_data.textureSlotIndex++] = texture;
    }

    for (uint32_t i = 0; i < 4; ++i) {
        s_data.quadVertexBufferPtr->position =
            transform * s_data.quadVertexPositions[i];
        s_data.quadVertexBufferPtr->color = color;
        s_data.quadVertexBufferPtr->texCoord = s_data.quadTexCoords[i];
        s_data.quadVertexBufferPtr->texIndex = textureIndex;
        s_data.quadVertexBufferPtr->grayScale =
            texture->getFormat() == TextureFormat::RED;
        s_data.quadVertexBufferPtr->entityIndex = -1;
        ++s_data.quadVertexBufferPtr;
    }
    s_data.quadIndexCnt += 6;
}

void Renderer2D::drawTexture(const Ref<Texture>& texture, const glm::vec3& pos,
                             const glm::vec3& scale, const glm::vec4& color) {
    drawTexture(texture,
                glm::translate(glm::mat4(1.0f), pos) *
                    glm::scale(glm::mat4(1.0f), scale),
                color);
}

void Renderer2D::drawText(const Font& font, const std::wstring& text,
                          float scale, const glm::vec4& color) {
    float x = s_data.textCursor.x;
    float y = s_data.textCursor.y;
    for (const auto c : text) {
        if (c == '\n') {
            x = s_data.textOrigin.x;
            y -= font.getPixelSize() * scale;
            continue;
        }
        const Character& ch = font.getCharacter(c);
        drawTexture(ch.texture,
                    glm::vec3(x + ch.bearing.x * scale,
                              y + (ch.bearing.y - ch.size.y) * scale, 0),
                    glm::vec3(ch.size.x * scale, ch.size.y * scale, 0), color);
        x += (ch.advance >> 6) * scale;
    }
    s_data.textCursor.x = x;
    s_data.textCursor.y = y;
}

void Renderer2D::setTextOrigin(float x, float y) {
    s_data.textOrigin.x = x;
    s_data.textOrigin.y = y;
    s_data.textCursor = s_data.textOrigin;
}

}  // namespace sd
