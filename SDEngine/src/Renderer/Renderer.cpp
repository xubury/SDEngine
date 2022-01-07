#include "Renderer/Renderer.hpp"
#include "Asset/AssetManager.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Device.hpp"
#include "Utility/String.hpp"
#include "Asset/Asset.hpp"

namespace SD {

const static std::array<glm::vec4, 4> QUAD_VERTEX_POS = {
    glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f), glm::vec4(0.5f, -0.5f, 0.0f, 1.0f),
    glm::vec4(0.5f, 0.5f, 0.0f, 1.0f), glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f)};
const static std::array<glm::vec2, 2> QUAD_UV = {glm::vec2(0, 0),
                                                 glm::vec2(1, 1)};

Renderer::Renderer(const Viewport& viewport, Device* device,
                   AssetManager* asset)
    : m_device(device), m_asset(asset), m_target(viewport) {
    SD_CORE_TRACE("Initializing Renderer");
    m_camera_UBO = UniformBuffer::Create(nullptr, sizeof(CameraData),
                                         BufferIOType::DYNAMIC);

    InitRenderer2D();

    m_target.AddTexture(
        TextureSpec(m_device->GetMSAA(), TextureType::TEX_2D_MULTISAMPLE,
                    DataFormat::RGBA, DataFormatType::UBYTE, TextureWrap::EDGE,
                    TextureMagFilter::NEAREST, TextureMinFilter::NEAREST));
    m_target.AddTexture(
        TextureSpec(m_device->GetMSAA(), TextureType::TEX_2D_MULTISAMPLE,
                    DataFormat::RED, DataFormatType::UINT, TextureWrap::EDGE,
                    TextureMagFilter::NEAREST, TextureMinFilter::NEAREST));
    m_target.AddTexture(TextureSpec(
        m_device->GetMSAA(), TextureType::TEX_2D_MULTISAMPLE, DataFormat::DEPTH,
        DataFormatType::FLOAT16, TextureWrap::EDGE));
    m_target.CreateFramebuffer();
}

Renderer::~Renderer() { SD_CORE_TRACE("Deleting Renderer"); }

void Renderer::InitRenderer2D() {
    // Initializing line vao
    m_data.line_vao = VertexArray::Create();
    auto line_vbo = VertexBuffer::Create(
        m_data.line_buffer.data(), m_data.line_buffer.size() * sizeof(Line),
        BufferIOType::DYNAMIC);

    VertexBufferLayout layout;
    layout.Push(BufferLayoutType::FLOAT3);
    layout.Push(BufferLayoutType::FLOAT4);
    layout.Push(BufferLayoutType::UINT);  // entity index
    m_data.line_vao->AddVertexBuffer(line_vbo, layout);

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
        m_data.quad_buffer.data(), m_data.quad_buffer.size() * sizeof(Quad),
        BufferIOType::DYNAMIC);

    layout.Clear();
    layout.Push(BufferLayoutType::FLOAT3);  // position
    layout.Push(BufferLayoutType::FLOAT4);  // color
    layout.Push(BufferLayoutType::FLOAT2);  // texCoord
    layout.Push(BufferLayoutType::INT);     // texIndex
    layout.Push(BufferLayoutType::UINT);    // entity index

    m_data.quad_vao = VertexArray::Create();
    m_data.quad_vao->AddVertexBuffer(quad_vbo, layout);
    m_data.quad_vao->SetIndexBuffer(quad_ebo);

    // Initialize circle vao
    auto circle_vbo = VertexBuffer::Create(
        m_data.circle_buffer.data(),
        m_data.circle_buffer.size() * sizeof(Circle), BufferIOType::DYNAMIC);

    layout.Clear();
    layout.Push(BufferLayoutType::FLOAT3);  // world_pos
    layout.Push(BufferLayoutType::FLOAT3);  // local_pos
    layout.Push(BufferLayoutType::FLOAT4);  // color
    layout.Push(BufferLayoutType::FLOAT);   // thickness
    layout.Push(BufferLayoutType::FLOAT);   // fade
    layout.Push(BufferLayoutType::UINT);    // entity index
    m_data.circle_vao = VertexArray::Create();
    m_data.circle_vao->AddVertexBuffer(circle_vbo, layout);
    m_data.circle_vao->SetIndexBuffer(quad_ebo);

    m_data.default_texture = Texture::Create(
        1, 1,
        TextureSpec(1, TextureType::TEX_2D, DataFormat::RGBA,
                    DataFormatType::FLOAT16, TextureWrap::REPEAT,
                    TextureMagFilter::LINEAR, TextureMinFilter::LINEAR));
    const float color[4] = {1, 1, 1, 1};
    m_data.default_texture->SetPixels(0, 0, 0, 1, 1, 1, color);

    m_data.texture_slots[0] = m_data.default_texture.get();

    m_line_shader = m_asset->LoadAndGet<Shader>("shaders/line.glsl");
    m_sprite_shader = m_asset->LoadAndGet<Shader>("shaders/sprite.glsl");
    m_circle_shader = m_asset->LoadAndGet<Shader>("shaders/circle.glsl");

    SetupShaderUBO(*m_line_shader);
    SetupShaderUBO(*m_sprite_shader);
    SetupShaderUBO(*m_circle_shader);
}

void Renderer::Submit(const Shader& shader, const VertexArray& vao,
                      MeshTopology topology, size_t count, size_t offset,
                      bool index) {
    m_device->SetShader(&shader);
    vao.Bind();
    if (index) {
        m_device->DrawElements(topology, count, offset);
    } else {
        m_device->DrawArrays(topology, offset, count);
    }
}

void Renderer::DrawMesh(const Shader& shader, const Mesh& mesh) {
    m_device->SetPolygonMode(mesh.GetPolygonMode(), Face::BOTH);
    VertexArray* vao = mesh.GetVertexArray();
    SD_CORE_ASSERT(vao, "Invalid mesh!");
    Renderer::Submit(shader, *vao, mesh.GetTopology(),
                     vao->GetIndexBuffer()->GetCount(), 0);
}

void Renderer::SetupShaderUBO(Shader& shader) {
    shader.SetUniformBuffer("Camera", *m_camera_UBO);
}

void Renderer::Begin(Shader& shader, Camera& camera) {
    m_camera_data.view = camera.GetView();
    m_camera_data.projection = camera.GetProjection();
    m_camera_UBO->UpdateData(&m_camera_data, sizeof(CameraData));

    SetupShaderUBO(shader);
}

void Renderer::Begin(Camera& camera) {
    m_camera_data.view = camera.GetView();
    m_camera_data.projection = camera.GetProjection();
    m_camera_UBO->UpdateData(&m_camera_data, sizeof(CameraData));
}

void Renderer::End() {
    Flush();
    StartBatch();
    m_device->SetShader(nullptr);
}

void Renderer::StartBatch() {
    // Reset text
    SetTextOrigin(0, 0);
    m_data.text_cursor.x = 0;
    m_data.text_cursor.y = 0;

    StartLineBatch();
    StartQuadBatch();
    StartCircleBatch();
}

void Renderer::StartLineBatch() {
    m_data.line_vertex_cnt = 0;
    m_data.line_buffer_ptr = m_data.line_buffer.data();
}

void Renderer::StartQuadBatch() {
    // Reset texture
    m_data.texture_index = 1;

    // Reset quad
    m_data.quad_index_cnt = 0;
    m_data.quad_buffer_ptr = m_data.quad_buffer.data();
}

void Renderer::StartCircleBatch() {
    // Reset circle
    m_data.circle_index_cnt = 0;
    m_data.circle_buffer_ptr = m_data.circle_buffer.data();
}

void Renderer::SetTextOrigin(int x, int y) {
    m_data.text_origin.x = x;
    m_data.text_origin.y = y;
}

glm::ivec2 Renderer::GetTextCursor() const {
    return m_data.text_cursor + m_data.text_origin;
}

void Renderer::Flush() {
    FlushLines();
    FlushQuads();
    FlushCircles();
}

void Renderer::FlushLines() {
    if (m_data.line_vertex_cnt) {
        size_t offset = m_data.line_buffer_ptr - m_data.line_buffer.data();
        m_data.line_vao->UpdateBuffer(0, m_data.line_buffer.data(),
                                      sizeof(Line) * offset);
        Submit(*m_line_shader, *m_data.line_vao, MeshTopology::LINES,
               m_data.line_vertex_cnt, 0, false);
    }
}

void Renderer::FlushQuads() {
    if (m_data.quad_index_cnt) {
        size_t offset = m_data.quad_buffer_ptr - m_data.quad_buffer.data();
        m_data.quad_vao->UpdateBuffer(0, m_data.quad_buffer.data(),
                                      offset * sizeof(Quad));

        for (uint32_t i = 0; i < m_data.texture_index; ++i) {
            m_sprite_shader->SetTexture(i, m_data.texture_slots[i]);
        }

        Submit(*m_sprite_shader, *m_data.quad_vao, MeshTopology::TRIANGLES,
               m_data.quad_index_cnt, 0);
    }
}

void Renderer::FlushCircles() {
    if (m_data.circle_index_cnt) {
        size_t offset = m_data.circle_buffer_ptr - m_data.circle_buffer.data();

        m_data.circle_vao->UpdateBuffer(0, m_data.circle_buffer.data(),
                                        offset * sizeof(Circle));

        Submit(*m_circle_shader, *m_data.circle_vao, MeshTopology::TRIANGLES,
               m_data.circle_index_cnt, 0);
    }
}

void Renderer::NextLineBatch() {
    FlushLines();
    StartLineBatch();
}

void Renderer::NextQuadBatch() {
    FlushQuads();
    StartQuadBatch();
}

void Renderer::NextCircleBatch() {
    FlushCircles();
    StartCircleBatch();
}

void Renderer::DrawLine(const glm::vec3& start, const glm::vec3& end,
                        const glm::vec4& color, uint32_t entity_id) {
    if (m_data.line_vertex_cnt >= Renderer2DData::MAX_LINES_VERTICES) {
        NextLineBatch();
    }
    m_data.line_buffer_ptr->vertices[0].pos = start;
    m_data.line_buffer_ptr->vertices[0].color = color;
    m_data.line_buffer_ptr->vertices[0].entity_id = entity_id;

    m_data.line_buffer_ptr->vertices[1].pos = end;
    m_data.line_buffer_ptr->vertices[1].color = color;
    m_data.line_buffer_ptr->vertices[1].entity_id = entity_id;

    ++m_data.line_buffer_ptr;
    m_data.line_vertex_cnt += 2;
}

void Renderer::DrawQuad(const glm::vec3& pos, const glm::quat& rot,
                        const glm::vec2& scale, const glm::vec4& color,
                        uint32_t entity_id) {
    DrawQuad(glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(rot) *
                 glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f)),
             color, entity_id);
}

void Renderer::DrawQuad(const glm::mat4& transform, const glm::vec4& color,
                        uint32_t entity_id) {
    if (m_data.quad_index_cnt >= Renderer2DData::MAX_INDICES) {
        NextQuadBatch();
    }
    for (uint32_t i = 0; i < 4; ++i) {
        glm::ivec2 uv_index = m_device->GetUVIndex(i);
        m_data.quad_buffer_ptr->vertices[i].position =
            transform * QUAD_VERTEX_POS[i];
        m_data.quad_buffer_ptr->vertices[i].color = color;
        m_data.quad_buffer_ptr->vertices[i].uv.x = QUAD_UV[uv_index.x].x;
        m_data.quad_buffer_ptr->vertices[i].uv.y = QUAD_UV[uv_index.y].y;
        m_data.quad_buffer_ptr->vertices[i].tex_id = 0;
        m_data.quad_buffer_ptr->vertices[i].entity_id = entity_id;
    }
    ++m_data.quad_buffer_ptr;
    m_data.quad_index_cnt += 6;
}

void Renderer::DrawTexture(const Texture& texture,
                           const std::array<glm::vec2, 2>& uv,
                           const glm::vec3& pos, const glm::quat& rot,
                           const glm::vec2& scale, const glm::vec4& color,
                           uint32_t entity_id) {
    DrawTexture(
        texture, uv,
        glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(rot) *
            glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f)),
        color, entity_id);
}

void Renderer::DrawTexture(const Texture& texture,
                           const std::array<glm::vec2, 2>& uv,
                           const glm::mat4& transform, const glm::vec4& color,
                           uint32_t entity_id) {
    if (m_data.quad_index_cnt >= Renderer2DData::MAX_INDICES) {
        NextQuadBatch();
    }

    uint32_t textureIndex = 0;
    for (uint32_t i = 1; i < m_data.texture_index; ++i) {
        if (*m_data.texture_slots[i] == texture) {
            textureIndex = i;
            break;
        }
    }

    if (textureIndex == 0) {
        if (m_data.texture_index >= Renderer2DData::MAX_TEXTURE_SLOTS) {
            NextQuadBatch();
        }
        textureIndex = m_data.texture_index;
        m_data.texture_slots[m_data.texture_index++] = &texture;
    }

    for (uint32_t i = 0; i < 4; ++i) {
        glm::ivec2 uv_index = m_device->GetUVIndex(i);
        m_data.quad_buffer_ptr->vertices[i].position =
            transform * QUAD_VERTEX_POS[i];
        m_data.quad_buffer_ptr->vertices[i].color = color;
        m_data.quad_buffer_ptr->vertices[i].uv.x = uv[uv_index.x].x;
        m_data.quad_buffer_ptr->vertices[i].uv.y = uv[uv_index.y].y;
        m_data.quad_buffer_ptr->vertices[i].tex_id = textureIndex;
        m_data.quad_buffer_ptr->vertices[i].entity_id = entity_id;
    }
    ++m_data.quad_buffer_ptr;
    m_data.quad_index_cnt += 6;
}

void Renderer::DrawTexture(const Texture& texture, const glm::vec3& pos,
                           const glm::quat& rot, const glm::vec2& scale,
                           const glm::vec4& color, uint32_t entity_id) {
    DrawTexture(
        texture, QUAD_UV,
        glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(rot) *
            glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f)),
        color, entity_id);
}

void Renderer::DrawTexture(const Texture& texture, const glm::mat4& transform,
                           const glm::vec4& color, uint32_t entity_id) {
    DrawTexture(texture, QUAD_UV, transform, color, entity_id);
}

void Renderer::DrawBillboard(const Texture& texture,
                             const std::array<glm::vec2, 2>& uv,
                             const glm::vec3& pos, const glm::vec2& scale,
                             const glm::vec4& color, uint32_t entity_id) {
    DrawTexture(
        texture, uv,
        glm::translate(glm::mat4(1.0f), pos) *
            glm::mat4(glm::transpose(glm::mat3(m_camera_data.view))) *
            glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f)),
        color, entity_id);
}

void Renderer::DrawBillboard(const Texture& texture, const glm::vec3& pos,
                             const glm::vec2& scale, const glm::vec4& color,
                             uint32_t entity_id) {
    DrawTexture(
        texture,
        glm::translate(glm::mat4(1.0f), pos) *
            glm::mat4(glm::transpose(glm::mat3(m_camera_data.view))) *
            glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f)),
        color, entity_id);
}

void Renderer::DrawText(const Font& font, const std::string& text,
                        const glm::mat4& transform, const glm::vec4& color,
                        uint32_t entity_id) {
    glm::mat4 t =
        glm::translate(glm::mat4(1.0f), glm::vec3(m_data.text_origin.x,
                                                  m_data.text_origin.y, 0)) *
        transform;
    std::u32string u32str = String::ConvertToUTF32(text);
    for (const auto c : u32str) {
        if (c == '\n') {
            m_data.text_cursor.x = 0;
            m_data.text_cursor.y -= font.GetPixelHeight();
            continue;
        } else if (c <= 128 && std::iscntrl(c)) {
            continue;
        }
        const Character& ch = font.GetCharacter(c);
        glm::mat4 offset =
            glm::translate(
                glm::mat4(1.0f),
                glm::vec3(
                    m_data.text_cursor.x + ch.bearing.x + ch.size.x * 0.5f,
                    m_data.text_cursor.y + ch.bearing.y - ch.size.y * 0.5f,
                    0)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(ch.size.x, ch.size.y, 1.0f));
        DrawTexture(*ch.glyph, ch.uv, t * offset, color, entity_id);
        m_data.text_cursor.x += ch.advance;
    }
}

void Renderer::DrawCircle(const glm::vec3& pos, const glm::vec2& scale,
                          const glm::vec4& color, float thickness, float fade,
                          uint32_t entity_id) {
    DrawCircle(
        glm::translate(glm::mat4(1.0f), pos) *
            glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f)),
        color, thickness, fade, entity_id);
}

void Renderer::DrawCircle(const glm::mat4& transform, const glm::vec4& color,
                          float thickness, float fade, uint32_t entity_id) {
    if (m_data.circle_index_cnt >= Renderer2DData::MAX_INDICES) {
        NextCircleBatch();
    }
    for (size_t i = 0; i < 4; ++i) {
        m_data.circle_buffer_ptr->vertices[i].world_pos =
            transform * QUAD_VERTEX_POS[i];
        m_data.circle_buffer_ptr->vertices[i].local_pos =
            QUAD_VERTEX_POS[i] * 2.f;
        m_data.circle_buffer_ptr->vertices[i].color = color;
        m_data.circle_buffer_ptr->vertices[i].thickness = thickness;
        m_data.circle_buffer_ptr->vertices[i].fade = fade;
        m_data.circle_buffer_ptr->vertices[i].entity_id = entity_id;
    }
    ++m_data.circle_buffer_ptr;
    m_data.circle_index_cnt += 6;
}

}  // namespace SD
