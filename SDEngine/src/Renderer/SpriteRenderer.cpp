#include "Renderer/SpriteRenderer.hpp"
#include "Loader/ShaderLoader.hpp"
#include "Utility/String.hpp"

namespace SD {

struct SD_RENDERER_API LineVertex {
    glm::vec3 pos;
    glm::vec4 color;
    uint32_t entity_id;
};

struct SD_RENDERER_API Line {
    std::array<LineVertex, 2> vertices;
};

struct SD_RENDERER_API QuadVertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 uv;
    int tex_id;
    uint32_t entity_id;
    QuadVertex() : position(0.f), color(0.f), uv(0.f), tex_id(0.f) {}
};

struct SD_RENDERER_API Quad {
    std::array<QuadVertex, 4> vertices;
    glm::vec3 GetCenter() const {
        return (vertices[0].position + vertices[1].position +
                vertices[2].position + vertices[3].position) /
               glm::vec3(4);
    }
};

struct SD_RENDERER_API CircleVertex {
    glm::vec3 world_pos;
    glm::vec3 local_pos;
    glm::vec4 color;
    float thickness;
    float fade;
    uint32_t entity_id;
};

struct SD_RENDERER_API Circle {
    std::array<CircleVertex, 4> vertices;
};

struct SD_RENDERER_API Renderer2DData {
    static const uint32_t MAX_QUADS = 20000;
    static const uint32_t MAX_LINES = 20000;
    static const uint32_t MAX_LINES_VERTICES = MAX_LINES * 2;
    static const uint32_t MAX_INDICES = MAX_QUADS * 6;
    static const uint32_t MAX_TEXTURE_SLOTS = 32;

    Ref<Texture> default_texture;

    Ref<VertexArray> line_vao;
    size_t line_vertex_cnt = 0;
    Ref<VertexBuffer> line_vbo;
    std::array<Line, MAX_LINES> line_buffer;
    Line* line_buffer_ptr = nullptr;

    Ref<VertexArray> quad_vao;
    Ref<VertexBuffer> quad_vbo;
    Ref<IndexBuffer> quad_ibo;
    size_t quad_index_cnt = 0;
    std::array<Quad, MAX_QUADS> quad_buffer;
    Quad* quad_buffer_ptr = nullptr;

    uint32_t texture_index = 1;
    std::array<const Texture*, MAX_TEXTURE_SLOTS> texture_slots;

    Ref<VertexArray> circle_vao;
    Ref<VertexBuffer> circle_vbo;
    size_t circle_index_cnt = 0;
    std::array<Circle, MAX_QUADS> circle_buffer;
    Circle* circle_buffer_ptr = nullptr;

    glm::ivec2 text_origin;
    glm::ivec2 text_cursor;
};

const static std::array<glm::vec4, 4> QUAD_VERTEX_POS = {
    glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f), glm::vec4(0.5f, -0.5f, 0.0f, 1.0f),
    glm::vec4(0.5f, 0.5f, 0.0f, 1.0f), glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f)};
const static std::array<glm::vec2, 4> QUAD_UV = {
    glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(0, 1)};

static Renderer2DData m_data;

static Ref<Shader> m_line_shader;
static Ref<Shader> m_circle_shader;
static Ref<Shader> m_sprite_shader;

void SpriteRenderer::Init() {
    // Initializing line vbo
    m_data.line_vbo = VertexBuffer::Create(
        m_data.line_buffer.data(), m_data.line_buffer.size() * sizeof(Line),
        BufferIOType::Dynamic);

    // Initializing line vao
    VertexBufferLayout layout;
    layout.Push(BufferLayoutType::Float3);
    layout.Push(BufferLayoutType::Float4);
    layout.Push(BufferLayoutType::UInt);  // entity index
    m_data.line_vao = VertexArray::Create();
    m_data.line_vao->AddBufferLayout(layout);
    m_data.line_vao->BindVertexBuffer(*m_data.line_vbo, 0);

    // Initializing quad ibo
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
    m_data.quad_ibo = IndexBuffer::Create(
        quad_indices.data(), quad_indices.size(), BufferIOType::Static);

    // Initialize quad vbo
    m_data.quad_vbo = VertexBuffer::Create(
        m_data.quad_buffer.data(), m_data.quad_buffer.size() * sizeof(Quad),
        BufferIOType::Dynamic);

    // Initialize quad vao
    layout.Clear();
    layout.Push(BufferLayoutType::Float3);  // position
    layout.Push(BufferLayoutType::Float4);  // color
    layout.Push(BufferLayoutType::Float2);  // texCoord
    layout.Push(BufferLayoutType::Int);     // texIndex
    layout.Push(BufferLayoutType::UInt);    // entity index
    m_data.quad_vao = VertexArray::Create();
    m_data.quad_vao->AddBufferLayout(layout);
    m_data.quad_vao->BindVertexBuffer(*m_data.quad_vbo, 0);
    m_data.quad_vao->BindIndexBuffer(*m_data.quad_ibo);

    // Initialize circle vbo
    m_data.circle_vbo = VertexBuffer::Create(
        m_data.circle_buffer.data(),
        m_data.circle_buffer.size() * sizeof(Circle), BufferIOType::Dynamic);

    // Initialize circle vao
    layout.Clear();
    layout.Push(BufferLayoutType::Float3);  // world_pos
    layout.Push(BufferLayoutType::Float3);  // local_pos
    layout.Push(BufferLayoutType::Float4);  // color
    layout.Push(BufferLayoutType::Float);   // thickness
    layout.Push(BufferLayoutType::Float);   // fade
    layout.Push(BufferLayoutType::UInt);    // entity index
    m_data.circle_vao = VertexArray::Create();
    m_data.circle_vao->AddBufferLayout(layout);
    m_data.circle_vao->BindVertexBuffer(*m_data.circle_vbo, 0);
    m_data.circle_vao->BindIndexBuffer(*m_data.quad_ibo);

    m_data.default_texture = Texture::Create(
        1, 1, 1, MultiSampleLevel::X1, TextureType::TEX_2D, DataFormat::RGBA32F,
        TextureWrap::Repeat, TextureMinFilter::Linear, MipmapMode::None,
        TextureMagFilter::Linear);
    const float color[4] = {1, 1, 1, 1};
    m_data.default_texture->SetPixels(0, 0, 0, 1, 1, 1, color);

    m_data.texture_slots[0] = m_data.default_texture.get();

    m_line_shader = ShaderLoader::LoadShader("assets/shaders/line.vert.glsl",
                                             "assets/shaders/line.frag.glsl");
    m_sprite_shader = ShaderLoader::LoadShader(
        "assets/shaders/texture.vert.glsl", "assets/shaders/texture.frag.glsl");
    m_circle_shader = ShaderLoader::LoadShader(
        "assets/shaders/circle.vert.glsl", "assets/shaders/circle.frag.glsl");

    SetupShaderUBO(*m_line_shader);
    SetupShaderUBO(*m_sprite_shader);
    SetupShaderUBO(*m_circle_shader);
}

void SpriteRenderer::SetupShaderUBO(Shader& shader) {
    shader.SetUniformBuffer("Camera", *m_camera_UBO);
}

void SpriteRenderer::Begin(Camera& camera) {
    m_camera_data.view = camera.GetView();
    m_camera_data.projection = camera.GetProjection();
    m_camera_UBO->UpdateData(&m_camera_data, sizeof(CameraData));
}

void SpriteRenderer::End() {
    Flush();
    StartBatch();
    m_device->SetShader(nullptr);
}

void SpriteRenderer::StartBatch() {
    // Reset text
    SetTextOrigin(0, 0);
    m_data.text_cursor.x = 0;
    m_data.text_cursor.y = 0;

    StartLineBatch();
    StartQuadBatch();
    StartCircleBatch();
}

void SpriteRenderer::StartLineBatch() {
    m_data.line_vertex_cnt = 0;
    m_data.line_buffer_ptr = m_data.line_buffer.data();
}

void SpriteRenderer::StartQuadBatch() {
    // Reset texture
    m_data.texture_index = 1;

    // Reset quad
    m_data.quad_index_cnt = 0;
    m_data.quad_buffer_ptr = m_data.quad_buffer.data();
}

void SpriteRenderer::StartCircleBatch() {
    // Reset circle
    m_data.circle_index_cnt = 0;
    m_data.circle_buffer_ptr = m_data.circle_buffer.data();
}

void SpriteRenderer::SetTextOrigin(int x, int y) {
    m_data.text_origin.x = x;
    m_data.text_origin.y = y;
}

glm::ivec2 SpriteRenderer::GetTextCursor() {
    return m_data.text_cursor + m_data.text_origin;
}

void SpriteRenderer::Flush() {
    FlushLines();
    FlushQuads();
    FlushCircles();
}

void SpriteRenderer::FlushLines() {
    if (m_data.line_vertex_cnt) {
        size_t offset = m_data.line_buffer_ptr - m_data.line_buffer.data();
        m_data.line_vbo->UpdateData(m_data.line_buffer.data(),
                                    sizeof(Line) * offset);
        m_device->SetShader(m_line_shader.get());
        Submit(*m_data.line_vao, MeshTopology::Lines, m_data.line_vertex_cnt, 0,
               false);
    }
}

void SpriteRenderer::FlushQuads() {
    if (m_data.quad_index_cnt) {
        size_t offset = m_data.quad_buffer_ptr - m_data.quad_buffer.data();
        m_data.quad_vbo->UpdateData(m_data.quad_buffer.data(),
                                    offset * sizeof(Quad));

        // int32_t texture_id_offset =
        //     m_sprite_shader->GetParam("u_textures[0]")->GetIndex();
        m_sprite_shader->GetParam("u_textures[0]")
            ->SetAsTextures(m_data.texture_slots.data(), m_data.texture_index);

        m_device->SetShader(m_sprite_shader.get());
        Submit(*m_data.quad_vao, MeshTopology::Triangles, m_data.quad_index_cnt,
               0);
    }
}

void SpriteRenderer::FlushCircles() {
    if (m_data.circle_index_cnt) {
        size_t offset = m_data.circle_buffer_ptr - m_data.circle_buffer.data();

        m_data.circle_vbo->UpdateData(m_data.circle_buffer.data(),
                                      offset * sizeof(Circle));

        m_device->SetShader(m_circle_shader.get());
        Submit(*m_data.circle_vao, MeshTopology::Triangles,
               m_data.circle_index_cnt, 0);
    }
}

void SpriteRenderer::NextLineBatch() {
    FlushLines();
    StartLineBatch();
}

void SpriteRenderer::NextQuadBatch() {
    FlushQuads();
    StartQuadBatch();
}

void SpriteRenderer::NextCircleBatch() {
    FlushCircles();
    StartCircleBatch();
}

void SpriteRenderer::DrawLine(const glm::vec3& start, const glm::vec3& end,
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

void SpriteRenderer::DrawQuad(const glm::vec3& pos, const glm::quat& rot,
                              const glm::vec2& scale, const glm::vec4& color,
                              uint32_t entity_id) {
    DrawQuad(glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(rot) *
                 glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f)),
             color, entity_id);
}

void SpriteRenderer::DrawQuad(const glm::mat4& transform,
                              const glm::vec4& color, uint32_t entity_id) {
    if (m_data.quad_index_cnt >= Renderer2DData::MAX_INDICES) {
        NextQuadBatch();
    }
    for (uint32_t i = 0; i < 4; ++i) {
        m_data.quad_buffer_ptr->vertices[i].position =
            transform * QUAD_VERTEX_POS[i];
        m_data.quad_buffer_ptr->vertices[i].color = color;
        m_data.quad_buffer_ptr->vertices[i].uv.x = QUAD_UV[i].x;
        m_data.quad_buffer_ptr->vertices[i].uv.y = QUAD_UV[i].y;
        m_data.quad_buffer_ptr->vertices[i].tex_id = 0;
        m_data.quad_buffer_ptr->vertices[i].entity_id = entity_id;
    }
    ++m_data.quad_buffer_ptr;
    m_data.quad_index_cnt += 6;
}

void SpriteRenderer::DrawTexture(const Texture& texture,
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

void SpriteRenderer::DrawTexture(const Texture& texture,
                                 const std::array<glm::vec2, 2>& uv,
                                 const glm::mat4& transform,
                                 const glm::vec4& color, uint32_t entity_id) {
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
        m_data.quad_buffer_ptr->vertices[i].position =
            transform * QUAD_VERTEX_POS[i];
        m_data.quad_buffer_ptr->vertices[i].color = color;
        m_data.quad_buffer_ptr->vertices[i].uv.x = uv[QUAD_UV[i].x].x;
        m_data.quad_buffer_ptr->vertices[i].uv.y = uv[QUAD_UV[i].y].y;
        m_data.quad_buffer_ptr->vertices[i].tex_id = textureIndex;
        m_data.quad_buffer_ptr->vertices[i].entity_id = entity_id;
    }
    ++m_data.quad_buffer_ptr;
    m_data.quad_index_cnt += 6;
}

void SpriteRenderer::DrawTexture(const Texture& texture, const glm::vec3& pos,
                                 const glm::quat& rot, const glm::vec2& scale,
                                 const glm::vec4& color, uint32_t entity_id) {
    DrawTexture(
        texture, {glm::vec2(0), glm::vec2(1)},
        glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(rot) *
            glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f)),
        color, entity_id);
}

void SpriteRenderer::DrawTexture(const Texture& texture,
                                 const glm::mat4& transform,
                                 const glm::vec4& color, uint32_t entity_id) {
    DrawTexture(texture, {glm::vec2(0), glm::vec2(1)}, transform, color,
                entity_id);
}

void SpriteRenderer::DrawBillboard(const Texture& texture,
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

void SpriteRenderer::DrawBillboard(const Texture& texture, const glm::vec3& pos,
                                   const glm::vec2& scale,
                                   const glm::vec4& color, uint32_t entity_id) {
    DrawTexture(
        texture,
        glm::translate(glm::mat4(1.0f), pos) *
            glm::mat4(glm::transpose(glm::mat3(m_camera_data.view))) *
            glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f)),
        color, entity_id);
}

void SpriteRenderer::DrawText(const Font& font, const std::string& text,
                              const glm::mat4& transform,
                              const glm::vec4& color, uint32_t entity_id) {
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

void SpriteRenderer::DrawCircle(const glm::vec3& pos, const glm::vec2& scale,
                                const glm::vec4& color, float thickness,
                                float fade, uint32_t entity_id) {
    DrawCircle(
        glm::translate(glm::mat4(1.0f), pos) *
            glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f)),
        color, thickness, fade, entity_id);
}

void SpriteRenderer::DrawCircle(const glm::mat4& transform,
                                const glm::vec4& color, float thickness,
                                float fade, uint32_t entity_id) {
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
