#include "Renderer/Renderer.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Device.hpp"
#include "Utility/String.hpp"
#include "Asset/Asset.hpp"

namespace SD {

const static uint8_t UV_INDEX[][2] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};

Renderer::Renderer(int width, int height, int msaa)
    : m_target(0, 0, width, height) {
    SD_CORE_TRACE("Initializing Renderer");
    m_camera_UBO = UniformBuffer::Create(nullptr, sizeof(CameraData),
                                         BufferIOType::DYNAMIC);

    if (msaa > 1) {
        Device::instance().Enable(Operation::MULTISAMPLE);
    } else {
        Device::instance().Disable(Operation::MULTISAMPLE);
    }
    InitRenderer2D();

    m_target.AddTexture(TextureSpec(msaa, TextureType::TEX_2D_MULTISAMPLE,
                                    DataFormat::RGBA, DataFormatType::UBYTE,
                                    TextureWrap::EDGE));
    m_target.AddTexture(TextureSpec(msaa, TextureType::TEX_2D_MULTISAMPLE,
                                    DataFormat::DEPTH, DataFormatType::FLOAT16,
                                    TextureWrap::EDGE));
    m_target.CreateFramebuffer();
}

void Renderer::InitRenderer2D() {
    // Initialize quad vao
    std::array<uint32_t, Renderer2DData::MAX_INDICES> quad_indices;
    uint32_t offset = 0;
    for (uint32_t i = 0; i < Renderer2DData::MAX_INDICES; i += 6) {
        quad_indices[i + 0] = offset + 0;
        quad_indices[i + 1] = offset + 1;
        quad_indices[i + 2] = offset + 2;

        quad_indices[i + 3] = offset + 2;
        quad_indices[i + 4] = offset + 3;
        quad_indices[i + 5] = offset + 0;

        offset += 4;
    }
    auto quad_ebo = IndexBuffer::Create(
        quad_indices.data(), quad_indices.size(), BufferIOType::STATIC);

    auto quad_vbo = VertexBuffer::Create(
        m_2d_data.quad_buffer.data(),
        m_2d_data.quad_buffer.size() * sizeof(Quad), BufferIOType::DYNAMIC);

    VertexBufferLayout layout;
    layout.Push(BufferLayoutType::FLOAT3);  // position
    layout.Push(BufferLayoutType::FLOAT4);  // color
    layout.Push(BufferLayoutType::FLOAT2);  // texCoord
    layout.Push(BufferLayoutType::FLOAT);   // texIndex

    m_2d_data.quad_vao = VertexArray::Create();
    m_2d_data.quad_vao->AddVertexBuffer(quad_vbo, layout);
    m_2d_data.quad_vao->SetIndexBuffer(quad_ebo);

    // Initialize circle vao
    auto circle_vbo = VertexBuffer::Create(
        m_2d_data.circle_buffer.data(),
        m_2d_data.circle_buffer.size() * sizeof(Circle), BufferIOType::DYNAMIC);

    layout.Clear();
    layout.Push(BufferLayoutType::FLOAT3);  // world_pos
    layout.Push(BufferLayoutType::FLOAT3);  // local_pos
    m_2d_data.circle_vao = VertexArray::Create();
    m_2d_data.circle_vao->AddVertexBuffer(circle_vbo, layout);
    m_2d_data.circle_vao->SetIndexBuffer(quad_ebo);

    m_2d_data.quad_vertex_pos[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
    m_2d_data.quad_vertex_pos[1] = {0.5f, -0.5f, 0.0f, 1.0f};
    m_2d_data.quad_vertex_pos[2] = {0.5f, 0.5f, 0.0f, 1.0f};
    m_2d_data.quad_vertex_pos[3] = {-0.5f, 0.5f, 0.0f, 1.0f};

    m_2d_data.quad_uv[0] = {0.0f, 1.0f};
    m_2d_data.quad_uv[1] = {1.0f, 0.0f};

    m_2d_data.texture_slots[0] = Texture::Create(
        1, 1,
        TextureSpec(1, TextureType::TEX_2D, DataFormat::RGBA,
                    DataFormatType::FLOAT16, TextureWrap::REPEAT,
                    TextureMagFilter::LINEAR, TextureMinFilter::LINEAR));
    const float color[4] = {1, 1, 1, 1};
    m_2d_data.texture_slots[0]->SetPixels(0, 0, 0, 1, 1, 1, color);

    m_2d_data.sprite_shader =
        ShaderLibrary::Instance().Load("shaders/sprite.glsl");
    m_2d_data.circle_shader =
        ShaderLibrary::Instance().Load("shaders/circle.glsl");
}

void Renderer::SetCamera(Camera* camera) { m_camera = camera; }

Camera* Renderer::GetCamera() { return m_camera; }

void Renderer::Submit(const VertexArray& vao, MeshTopology topology,
                      size_t count, size_t offset) {
    vao.Bind();
    Device::instance().DrawElements(topology, count, offset);
}

void Renderer::DrawMesh(const Mesh& mesh) {
    Device::instance().SetPolygonMode(mesh.GetPolygonMode(), Face::BOTH);
    VertexArray* vao = mesh.GetVertexArray();
    SD_CORE_ASSERT(vao, "Invalid mesh!");
    Renderer::Submit(*vao, mesh.GetTopology(),
                     vao->GetIndexBuffer()->GetCount(), 0);
}

void Renderer::Begin(Shader& shader, Camera& camera) {
    CameraData cameraData;
    cameraData.view = camera.GetView();
    cameraData.projection = camera.GetProjection();
    m_camera_UBO->UpdateData(&cameraData, sizeof(CameraData));
    shader.SetUniformBuffer("Camera", *m_camera_UBO);
}

void Renderer::Begin(Camera& camera) {
    Begin(*m_2d_data.sprite_shader, camera);
    StartBatch();
}

void Renderer::End() { Flush(); }

void Renderer::Flush() {
    FlushQuads();
    FlushCircles();
    StartBatch();
}

void Renderer::FlushQuads() {
    if (m_2d_data.quad_index_cnt) {
        size_t offset =
            m_2d_data.quad_buffer_ptr - m_2d_data.quad_buffer.data();
        glm::vec3 viewPos = m_camera->GetWorldPosition();

        // sort the drawing order to render transparent texture properly.
        std::sort(m_2d_data.quad_buffer.begin(),
                  m_2d_data.quad_buffer.begin() + offset,
                  [&viewPos](const Quad& lhs, const Quad& rhs) {
                      return glm::distance2(lhs.GetCenter(), viewPos) >
                             glm::distance2(rhs.GetCenter(), viewPos);
                  });

        m_2d_data.quad_vao->UpdateBuffer(0, m_2d_data.quad_buffer.data(),
                                         offset * sizeof(Quad));

        for (uint32_t i = 0; i < m_2d_data.texture_index; ++i) {
            m_2d_data.sprite_shader->SetTexture(
                i, m_2d_data.texture_slots[i].get());
        }

        m_2d_data.sprite_shader->Bind();
        Submit(*m_2d_data.quad_vao, MeshTopology::TRIANGLES,
               m_2d_data.quad_index_cnt, 0);
    }
}

void Renderer::FlushCircles() {
    if (m_2d_data.circle_index_cnt) {
        size_t offset =
            m_2d_data.circle_buffer_ptr - m_2d_data.circle_buffer.data();

        m_2d_data.circle_vao->UpdateBuffer(0, m_2d_data.circle_buffer.data(),
                                           offset * sizeof(Circle));

        m_2d_data.circle_shader->Bind();
        Submit(*m_2d_data.circle_vao, MeshTopology::TRIANGLES,
               m_2d_data.circle_index_cnt, 0);
    }
}

void Renderer::StartBatch() {
    SetTextOrigin(0, 0);
    m_2d_data.texture_index = 1;

    m_2d_data.quad_index_cnt = 0;
    m_2d_data.quad_buffer_ptr = m_2d_data.quad_buffer.data();

    m_2d_data.circle_index_cnt = 0;
    m_2d_data.circle_buffer_ptr = m_2d_data.circle_buffer.data();
}

void Renderer::NextBatch() {
    Flush();
    StartBatch();
}

void Renderer::SetTextOrigin(float x, float y) {
    m_2d_data.textOrigin.x = x;
    m_2d_data.textOrigin.y = y;
    m_2d_data.text_cursor.x = 0;
    m_2d_data.text_cursor.y = 0;
}

void Renderer::DrawQuad(const glm::mat4& transform, const glm::vec4& color) {
    if (m_2d_data.quad_index_cnt >= Renderer2DData::MAX_INDICES) {
        NextBatch();
    }
    for (uint32_t i = 0; i < 4; ++i) {
        m_2d_data.quad_buffer_ptr->vertices[i].position =
            transform * m_2d_data.quad_vertex_pos[i];
        m_2d_data.quad_buffer_ptr->vertices[i].color = color;
        m_2d_data.quad_buffer_ptr->vertices[i].uv.x =
            m_2d_data.quad_uv[UV_INDEX[i][0]].x;
        m_2d_data.quad_buffer_ptr->vertices[i].uv.y =
            m_2d_data.quad_uv[UV_INDEX[i][0]].y;
        m_2d_data.quad_buffer_ptr->vertices[i].tex_id = 0;
    }
    ++m_2d_data.quad_buffer_ptr;
    m_2d_data.quad_index_cnt += 6;
}

void Renderer::DrawTexture(const Ref<Texture>& texture,
                           const glm::mat4& transform,
                           const std::array<glm::vec2, 2>& uv,
                           const glm::vec4& color) {
    if (m_2d_data.quad_index_cnt >= Renderer2DData::MAX_INDICES) {
        NextBatch();
    }

    uint32_t textureIndex = 0;
    for (uint32_t i = 1; i < m_2d_data.texture_index; ++i) {
        if (*m_2d_data.texture_slots[i] == *texture) {
            textureIndex = i;
            break;
        }
    }

    if (textureIndex == 0) {
        if (m_2d_data.texture_index >= Renderer2DData::MAX_TEXTURE_SLOTS) {
            NextBatch();
        }
        textureIndex = m_2d_data.texture_index;
        m_2d_data.texture_slots[m_2d_data.texture_index++] = texture;
    }

    for (uint32_t i = 0; i < 4; ++i) {
        m_2d_data.quad_buffer_ptr->vertices[i].position =
            transform * m_2d_data.quad_vertex_pos[i];
        m_2d_data.quad_buffer_ptr->vertices[i].color = color;
        m_2d_data.quad_buffer_ptr->vertices[i].uv.x = uv[UV_INDEX[i][0]].x;
        m_2d_data.quad_buffer_ptr->vertices[i].uv.y = uv[UV_INDEX[i][1]].y;
        m_2d_data.quad_buffer_ptr->vertices[i].tex_id = textureIndex;
    }
    ++m_2d_data.quad_buffer_ptr;
    m_2d_data.quad_index_cnt += 6;
}

void Renderer::DrawTexture(const Ref<Texture>& texture,
                           const glm::mat4& transform, const glm::vec4& color) {
    DrawTexture(texture, transform, m_2d_data.quad_uv, color);
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
            m_2d_data.text_cursor.x = 0;
            m_2d_data.text_cursor.y -= pixelSize;
            continue;
        } else if (c <= 128 && std::iscntrl(c)) {
            continue;
        }
        const Character& ch = font.GetCharacter(c, pixelSize);
        glm::mat4 offset =
            glm::translate(
                glm::mat4(1.0f),
                glm::vec3(
                    m_2d_data.text_cursor.x + ch.bearing.x + ch.size.x * 0.5f,
                    m_2d_data.text_cursor.y + ch.bearing.y - ch.size.y * 0.5f,
                    0)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(ch.size.x, ch.size.y, 1.0f));
        DrawTexture(ch.glyph, t * offset, ch.uv, color);
        m_2d_data.text_cursor.x += ch.advance;
    }
}

void Renderer::DrawCircle(const glm::mat4& transform) {
    if (m_2d_data.circle_index_cnt >= Renderer2DData::MAX_INDICES) {
        NextBatch();
    }
    for (size_t i = 0; i < 4; ++i) {
        m_2d_data.circle_buffer_ptr->vertices[i].world_pos =
            transform * m_2d_data.quad_vertex_pos[i];
        m_2d_data.circle_buffer_ptr->vertices[i].local_pos =
            m_2d_data.quad_vertex_pos[i];
    }
    ++m_2d_data.circle_buffer_ptr;
    m_2d_data.circle_index_cnt += 6;
}

void Renderer::RenderToScreen() {
    Device::instance().BlitFramebuffer(m_target.GetFramebuffer(), 0, nullptr, 0,
                                       BufferBitMask::COLOR_BUFFER_BIT,
                                       TextureMagFilter::NEAREST);
}

}  // namespace SD
