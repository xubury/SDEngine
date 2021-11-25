#include "Renderer/Renderer.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Device.hpp"
#include "Utility/String.hpp"
#include "Asset/Asset.hpp"

namespace SD {

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

struct CameraData {
    glm::mat4 viewProjection;
    glm::vec3 viewPos;
};

struct Renderer2DData {
    static const uint32_t MAX_QUADS = 20000;
    static const uint32_t MAX_VERTICES = MAX_QUADS * 4;
    static const uint32_t MAX_INDICES = MAX_QUADS * 6;
    static const uint32_t MAX_TEXTURE_SLOTS = 32;

    Ref<VertexArray> quadVAO;
    Ref<VertexBuffer> quadVBO;
    Ref<IndexBuffer> quadEBO;
    Ref<Shader> spriteShader;

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

static Renderer2DData s_data;

Renderer::Renderer(AssetManager* manager, int msaa) {
    m_cameraUBO = UniformBuffer::Create(nullptr, sizeof(CameraData),
                                        BufferIOType::DYNAMIC);

    m_device = Device::Create();
    if (msaa > 1) {
        m_device->Enable(Operation::MULTISAMPLE);
    } else {
        m_device->Disable(Operation::MULTISAMPLE);
    }
    InitRenderer2D(manager);
}

void Renderer::InitRenderer2D(AssetManager* manager) {
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
    s_data.quadEBO = IndexBuffer::Create(quadIndices.data(), quadIndices.size(),
                                         BufferIOType::STATIC);

    s_data.quadVBO =
        VertexBuffer::Create(s_data.quadVertexBufferBase.data(),
                             s_data.quadVertexBufferBase.size() * sizeof(Quad),
                             BufferIOType::DYNAMIC);

    VertexBufferLayout layout;
    layout.Push(BufferDataType::FLOAT, 3);  // position
    layout.Push(BufferDataType::FLOAT, 4);  // color
    layout.Push(BufferDataType::FLOAT, 2);  // texCoord
    layout.Push(BufferDataType::FLOAT, 1);  // texIndex

    s_data.quadVAO = VertexArray::Create();
    s_data.quadVAO->AddVertexBuffer(s_data.quadVBO, layout);
    s_data.quadVAO->SetIndexBuffer(s_data.quadEBO);

    s_data.quadVertexPositions[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
    s_data.quadVertexPositions[1] = {0.5f, -0.5f, 0.0f, 1.0f};
    s_data.quadVertexPositions[2] = {0.5f, 0.5f, 0.0f, 1.0f};
    s_data.quadVertexPositions[3] = {-0.5f, 0.5f, 0.0f, 1.0f};

    s_data.quadTexCoords[0] = {0.0f, 1.0f};
    s_data.quadTexCoords[1] = {1.0f, 1.0f};
    s_data.quadTexCoords[2] = {1.0f, 0.0f};
    s_data.quadTexCoords[3] = {0.0f, 0.0f};

    const float color[4] = {1, 1, 1, 1};
    s_data.textureSlots[0] = Texture::Create(
        1, 1, 1, TextureType::TEX_2D, TextureFormat::RGBA,
        TextureFormatType::FLOAT, TextureWrap::REPEAT, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR, &color);

    s_data.spriteShader = manager->LoadAndGet<Shader>("shaders/sprite.glsl");
}

void Renderer::SetRenderTarget(RenderTarget& target) {
    m_device->SetFramebuffer(target.GetFramebuffer());
    m_device->SetViewport(target.GetX(), target.GetY(), target.GetWidth(),
                          target.GetHeight());
}

void Renderer::UpdateShader(Shader& shader, Camera& camera) {
    CameraData cameraData;
    cameraData.viewPos = camera.GetWorldPosition();
    cameraData.viewProjection = camera.GetViewPorjection();
    m_cameraUBO->UpdateData(&cameraData, sizeof(CameraData));
    shader.SetUniformBuffer("Camera", *m_cameraUBO);
}

void Renderer::SetScene(Scene* scene) { m_scene = scene; }

Scene* Renderer::GetScene() { return m_scene; }

void Renderer::SetCamera(Camera* camera) { m_camera = camera; }

Camera* Renderer::GetCamera() { return m_camera; }

void Renderer::Submit(const VertexArray& vao, MeshTopology topology,
                      size_t count, size_t offset) {
    vao.Bind();
    m_device->DrawElements(topology, count, offset);
}

void Renderer::DrawMesh(const Mesh& mesh) {
    m_device->SetPolygonMode(mesh.GetPolygonMode(), Face::BOTH);
    VertexArray* vao = mesh.GetVertexArray();
    SD_CORE_ASSERT(vao, "Invalid mesh!");
    Renderer::Submit(*vao, mesh.GetTopology(),
                     vao->GetIndexBuffer()->GetCount(), 0);
}

void Renderer::BeginScene(Camera& camera) {
    UpdateShader(*s_data.spriteShader, camera);
    StartBatch();
}

void Renderer::EndScene() { Flush(); }

void Renderer::Flush() {
    if (s_data.quadIndexCnt == 0) {
        return;
    }
    size_t offset =
        s_data.quadVertexBufferPtr - s_data.quadVertexBufferBase.data();
    glm::vec3 viewPos = m_camera->GetWorldPosition();

    // sort the drawing order to render transparent texture properly.
    std::sort(s_data.quadVertexBufferBase.begin(),
              s_data.quadVertexBufferBase.begin() + offset,
              [&viewPos](const Quad& lhs, const Quad& rhs) {
                  return glm::distance2(lhs.getCenter(), viewPos) >
                         glm::distance2(rhs.getCenter(), viewPos);
              });

    s_data.quadVBO->UpdateData(s_data.quadVertexBufferBase.data(),
                               offset * sizeof(Quad));

    for (uint32_t i = 0; i < s_data.textureSlotIndex; ++i) {
        s_data.spriteShader->SetTexture(i, s_data.textureSlots[i].get());
    }

    s_data.spriteShader->Bind();
    Renderer::Submit(*s_data.quadVAO, MeshTopology::TRIANGLES,
                     s_data.quadIndexCnt, 0);

    SetTextOrigin(0, 0);
}

void Renderer::StartBatch() {
    s_data.quadIndexCnt = 0;
    s_data.textureSlotIndex = 1;
    s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase.data();
}

void Renderer::NextBatch() {
    Flush();
    StartBatch();
}

void Renderer::SetTextOrigin(float x, float y) {
    s_data.textOrigin.x = x;
    s_data.textOrigin.y = y;
    s_data.textCursor.x = 0;
    s_data.textCursor.y = 0;
}

void Renderer::DrawQuad(const glm::mat4& transform, const glm::vec4& color) {
    if (s_data.quadIndexCnt >= Renderer2DData::MAX_INDICES) {
        NextBatch();
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

void Renderer::DrawTexture(const Ref<Texture>& texture,
                           const glm::mat4& transform,
                           const std::array<glm::vec2, 4>& texCoords,
                           const glm::vec4& color) {
    if (s_data.quadIndexCnt >= Renderer2DData::MAX_INDICES) {
        NextBatch();
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
            NextBatch();
        }
        textureIndex = s_data.textureSlotIndex;
        s_data.textureSlots[s_data.textureSlotIndex++] = texture;
    }

    for (uint32_t i = 0; i < 4; ++i) {
        s_data.quadVertexBufferPtr->vertices[i].position =
            transform * s_data.quadVertexPositions[i];
        s_data.quadVertexBufferPtr->vertices[i].color = color;
        s_data.quadVertexBufferPtr->vertices[i].texCoord = texCoords[i];
        s_data.quadVertexBufferPtr->vertices[i].texIndex = textureIndex;
    }
    ++s_data.quadVertexBufferPtr;
    s_data.quadIndexCnt += 6;
}

void Renderer::DrawTexture(const Ref<Texture>& texture,
                           const glm::mat4& transform, const glm::vec4& color) {
    DrawTexture(texture, transform, s_data.quadTexCoords, color);
}

void Renderer::DrawBillboard(const Ref<Texture>& texture, const glm::vec3& pos,
                             const glm::vec2& scale, const glm::vec4& color) {
    DrawTexture(
        texture,
        glm::translate(glm::mat4(1.0f), pos) *
            glm::toMat4(m_camera->GetWorldRotation()) *
            glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f)),
        color);
}

void Renderer::DrawText(Font& font, const std::string& text, uint8_t pixelSize,
                        const glm::mat4& transform, const glm::vec4& color) {
    glm::mat4 t =
        glm::translate(glm::mat4(1.0f),
                       glm::vec3(s_data.textOrigin.x, s_data.textOrigin.y, 0)) *
        transform;
    std::u32string u32str = String::ConvertToUTF32(text);
    for (const auto c : u32str) {
        if (c == '\n') {
            s_data.textCursor.x = 0;
            s_data.textCursor.y -= pixelSize;
            continue;
        } else if (c <= 128 && std::iscntrl(c)) {
            continue;
        }
        const Character& ch = font.GetCharacter(c, pixelSize);
        glm::mat4 offset =
            glm::translate(
                glm::mat4(1.0f),
                glm::vec3(s_data.textCursor.x + ch.bearing.x + ch.size.x * 0.5f,
                          s_data.textCursor.y + ch.bearing.y - ch.size.y * 0.5f,
                          0)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(ch.size.x, ch.size.y, 1.0f));
        DrawTexture(ch.texture, t * offset, ch.texCoord, color);
        s_data.textCursor.x += ch.advance;
    }
}

}  // namespace SD
