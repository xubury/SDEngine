#include "Renderer/Renderer.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Device.hpp"
#include "Utility/String.hpp"
#include "Asset/Asset.hpp"

namespace SD {

Renderer::Renderer(AssetManager* manager, int msaa) {
    SD_CORE_TRACE("Initializing Renderer");
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
    std::array<uint32_t, Renderer2DData::MAX_INDICES> quadIndices;
    uint32_t offset = 0;
    for (uint32_t i = 0; i < Renderer2DData::MAX_INDICES; i += 6) {
        quadIndices[i + 0] = offset + 0;
        quadIndices[i + 1] = offset + 1;
        quadIndices[i + 2] = offset + 2;

        quadIndices[i + 3] = offset + 2;
        quadIndices[i + 4] = offset + 3;
        quadIndices[i + 5] = offset + 0;

        offset += 4;
    }
    m_2d_data.quadEBO = IndexBuffer::Create(
        quadIndices.data(), quadIndices.size(), BufferIOType::STATIC);

    m_2d_data.quadVBO = VertexBuffer::Create(
        m_2d_data.quadVertexBufferBase.data(),
        m_2d_data.quadVertexBufferBase.size() * sizeof(Quad),
        BufferIOType::DYNAMIC);

    VertexBufferLayout layout;
    layout.Push(BufferDataType::FLOAT, 3);  // position
    layout.Push(BufferDataType::FLOAT, 4);  // color
    layout.Push(BufferDataType::FLOAT, 2);  // texCoord
    layout.Push(BufferDataType::FLOAT, 1);  // texIndex

    m_2d_data.quadVAO = VertexArray::Create();
    m_2d_data.quadVAO->AddVertexBuffer(m_2d_data.quadVBO, layout);
    m_2d_data.quadVAO->SetIndexBuffer(m_2d_data.quadEBO);

    m_2d_data.quadVertexPositions[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
    m_2d_data.quadVertexPositions[1] = {0.5f, -0.5f, 0.0f, 1.0f};
    m_2d_data.quadVertexPositions[2] = {0.5f, 0.5f, 0.0f, 1.0f};
    m_2d_data.quadVertexPositions[3] = {-0.5f, 0.5f, 0.0f, 1.0f};

    m_2d_data.quadTexCoords[0] = {0.0f, 1.0f};
    m_2d_data.quadTexCoords[1] = {1.0f, 0.0f};

    const float color[4] = {1, 1, 1, 1};
    m_2d_data.textureSlots[0] = Texture::Create(
        1, 1, 1, TextureType::TEX_2D, TextureFormat::RGBA,
        TextureFormatType::FLOAT, TextureWrap::REPEAT, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR, &color);

    m_2d_data.spriteShader = manager->LoadAndGet<Shader>("shaders/sprite.glsl");
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
    UpdateShader(*m_2d_data.spriteShader, camera);
    StartBatch();
}

void Renderer::EndScene() { Flush(); }

void Renderer::Flush() {
    if (m_2d_data.quadIndexCnt == 0) {
        return;
    }
    size_t offset =
        m_2d_data.quadVertexBufferPtr - m_2d_data.quadVertexBufferBase.data();
    glm::vec3 viewPos = m_camera->GetWorldPosition();

    // sort the drawing order to render transparent texture properly.
    std::sort(m_2d_data.quadVertexBufferBase.begin(),
              m_2d_data.quadVertexBufferBase.begin() + offset,
              [&viewPos](const Quad& lhs, const Quad& rhs) {
                  return glm::distance2(lhs.GetCenter(), viewPos) >
                         glm::distance2(rhs.GetCenter(), viewPos);
              });

    m_2d_data.quadVBO->UpdateData(m_2d_data.quadVertexBufferBase.data(),
                                  offset * sizeof(Quad));

    for (uint32_t i = 0; i < m_2d_data.textureSlotIndex; ++i) {
        m_2d_data.spriteShader->SetTexture(i, m_2d_data.textureSlots[i].get());
    }

    m_2d_data.spriteShader->Bind();
    Renderer::Submit(*m_2d_data.quadVAO, MeshTopology::TRIANGLES,
                     m_2d_data.quadIndexCnt, 0);

    SetTextOrigin(0, 0);
}

void Renderer::StartBatch() {
    m_2d_data.quadIndexCnt = 0;
    m_2d_data.textureSlotIndex = 1;
    m_2d_data.quadVertexBufferPtr = m_2d_data.quadVertexBufferBase.data();
}

void Renderer::NextBatch() {
    Flush();
    StartBatch();
}

void Renderer::SetTextOrigin(float x, float y) {
    m_2d_data.textOrigin.x = x;
    m_2d_data.textOrigin.y = y;
    m_2d_data.textCursor.x = 0;
    m_2d_data.textCursor.y = 0;
}

void Renderer::DrawQuad(const glm::mat4& transform, const glm::vec4& color) {
    if (m_2d_data.quadIndexCnt >= Renderer2DData::MAX_INDICES) {
        NextBatch();
    }
    for (uint32_t i = 0; i < 4; ++i) {
        m_2d_data.quadVertexBufferPtr->vertices[i].position =
            transform * m_2d_data.quadVertexPositions[i];
        m_2d_data.quadVertexBufferPtr->vertices[i].color = color;
        m_2d_data.quadVertexBufferPtr->vertices[i].texCoord =
            m_2d_data.quadTexCoords[i];
        m_2d_data.quadVertexBufferPtr->vertices[i].texIndex = 0;
    }
    ++m_2d_data.quadVertexBufferPtr;
    m_2d_data.quadIndexCnt += 6;
}

void Renderer::DrawTexture(const Ref<Texture>& texture,
                           const glm::mat4& transform,
                           const std::array<glm::vec2, 2>& texCoords,
                           const glm::vec4& color) {
    if (m_2d_data.quadIndexCnt >= Renderer2DData::MAX_INDICES) {
        NextBatch();
    }

    uint32_t textureIndex = 0;
    for (uint32_t i = 1; i < m_2d_data.textureSlotIndex; ++i) {
        if (*m_2d_data.textureSlots[i] == *texture) {
            textureIndex = i;
            break;
        }
    }

    if (textureIndex == 0) {
        if (m_2d_data.textureSlotIndex >= Renderer2DData::MAX_TEXTURE_SLOTS) {
            NextBatch();
        }
        textureIndex = m_2d_data.textureSlotIndex;
        m_2d_data.textureSlots[m_2d_data.textureSlotIndex++] = texture;
    }

    for (uint32_t i = 0; i < 4; ++i) {
        m_2d_data.quadVertexBufferPtr->vertices[i].position =
            transform * m_2d_data.quadVertexPositions[i];
        m_2d_data.quadVertexBufferPtr->vertices[i].color = color;
        m_2d_data.quadVertexBufferPtr->vertices[i].texCoord.x =
            texCoords[i == 1 || i == 2].x;
        m_2d_data.quadVertexBufferPtr->vertices[i].texCoord.y =
            texCoords[i >= 2].y;
        m_2d_data.quadVertexBufferPtr->vertices[i].texIndex = textureIndex;
    }
    ++m_2d_data.quadVertexBufferPtr;
    m_2d_data.quadIndexCnt += 6;
}

void Renderer::DrawTexture(const Ref<Texture>& texture,
                           const glm::mat4& transform, const glm::vec4& color) {
    DrawTexture(texture, transform, m_2d_data.quadTexCoords, color);
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
        glm::translate(glm::mat4(1.0f), glm::vec3(m_2d_data.textOrigin.x,
                                                  m_2d_data.textOrigin.y, 0)) *
        transform;
    std::u32string u32str = String::ConvertToUTF32(text);
    for (const auto c : u32str) {
        if (c == '\n') {
            m_2d_data.textCursor.x = 0;
            m_2d_data.textCursor.y -= pixelSize;
            continue;
        } else if (c <= 128 && std::iscntrl(c)) {
            continue;
        }
        const Character& ch = font.GetCharacter(c, pixelSize);
        glm::mat4 offset =
            glm::translate(
                glm::mat4(1.0f),
                glm::vec3(
                    m_2d_data.textCursor.x + ch.bearing.x + ch.size.x * 0.5f,
                    m_2d_data.textCursor.y + ch.bearing.y - ch.size.y * 0.5f,
                    0)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(ch.size.x, ch.size.y, 1.0f));
        DrawTexture(ch.glyph, t * offset, ch.texCoords, color);
        m_2d_data.textCursor.x += ch.advance;
    }
}

}  // namespace SD
