#include "Renderer/Renderer2D.hpp"
#include "Resource/ShaderManager.hpp"
#include "Utility/String.hpp"

namespace SD {

struct SD_RENDERER_API LineVertex {
    Vector3f pos;
    Vector4f color;
    uint32_t entity_id;
};

struct SD_RENDERER_API Line {
    std::array<LineVertex, 2> vertices;
};

struct SD_RENDERER_API QuadVertex {
    Vector3f position;
    Vector4f color;
    Vector2f uv;
    int tex_id;
    uint32_t entity_id;
    QuadVertex() : position(0.f), color(0.f), uv(0.f), tex_id(0.f) {}
};

struct SD_RENDERER_API Quad {
    std::array<QuadVertex, 4> vertices;
    Vector3f GetCenter() const
    {
        return (vertices[0].position + vertices[1].position +
                vertices[2].position + vertices[3].position) /
               Vector3f(4);
    }
};

struct SD_RENDERER_API CircleVertex {
    Vector3f world_pos;
    Vector3f local_pos;
    Vector4f color;
    float thickness;
    float fade;
    uint32_t entity_id;
};

struct SD_RENDERER_API Circle {
    std::array<CircleVertex, 4> vertices;
};

struct SD_RENDERER_API Renderer2DData {
    static const uint32_t MAX_QUADS{20000};
    static const uint32_t MAX_LINES{20000};
    static const uint32_t MAX_LINES_VERTICES{MAX_LINES * 2};
    static const uint32_t MAX_INDICES{MAX_QUADS * 6};
    static const uint32_t MAX_TEXTURE_SLOTS{32};

    Ref<Texture> default_texture;

    Ref<VertexArray> line_vao;
    size_t line_vertex_cnt{0};
    size_t line_draw_call{0};
    size_t line_cnt{0};
    Ref<VertexBuffer> line_vbo;
    std::array<Line, MAX_LINES> line_buffer;
    Line* line_buffer_ptr{nullptr};

    Ref<VertexArray> quad_vao;
    Ref<VertexBuffer> quad_vbo;
    Ref<IndexBuffer> quad_ibo;
    size_t quad_index_cnt{0};
    size_t quad_cnt{0};
    size_t quad_draw_call{0};
    std::array<Quad, MAX_QUADS> quad_buffer;
    Quad* quad_buffer_ptr = nullptr;

    uint32_t texture_index{1};
    std::array<const Texture*, MAX_TEXTURE_SLOTS> texture_slots;

    Ref<VertexArray> circle_vao;
    Ref<VertexBuffer> circle_vbo;
    size_t circle_index_cnt{0};
    size_t circie_cnt{0};
    size_t circle_draw_call{0};
    std::array<Circle, MAX_QUADS> circle_buffer;
    Circle* circle_buffer_ptr{nullptr};

    Vector2i text_origin;
    Vector2i text_cursor;
};

const static std::array<Vector4f, 4> QUAD_VERTEX_POS = {
    Vector4f(-0.5f, -0.5f, 0.0f, 1.0f), Vector4f(0.5f, -0.5f, 0.0f, 1.0f),
    Vector4f(0.5f, 0.5f, 0.0f, 1.0f), Vector4f(-0.5f, 0.5f, 0.0f, 1.0f)};
// OpenGL define uv coordinate origin's at bottom-left.
const static std::array<Vector2f, 4> QUAD_UV = {Vector2f(0, 1), Vector2f(1, 1),
                                                Vector2f(1, 0), Vector2f(0, 0)};

static Renderer2DData s_2d_data;

static ShaderHandle s_line_shader;
static ShaderHandle s_cirlce_shader;
static ShaderHandle s_texture_shader;

void Renderer2D::Init()
{
    // Initializing line vbo
    s_2d_data.line_vbo = VertexBuffer::Create(
        s_2d_data.line_buffer.data(),
        s_2d_data.line_buffer.size() * sizeof(Line), BufferIOType::Dynamic);

    // Initializing line vao
    VertexBufferLayout layout;
    layout.Push(BufferLayoutType::Float3);
    layout.Push(BufferLayoutType::Float4);
    layout.Push(BufferLayoutType::UInt);  // entity index
    s_2d_data.line_vao = VertexArray::Create();
    s_2d_data.line_vao->AddBufferLayout(layout);
    s_2d_data.line_vao->BindVertexBuffer(*s_2d_data.line_vbo, 0);

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
    s_2d_data.quad_ibo = IndexBuffer::Create(
        quad_indices.data(), quad_indices.size(), BufferIOType::Static);

    // Initialize quad vbo
    s_2d_data.quad_vbo = VertexBuffer::Create(
        s_2d_data.quad_buffer.data(),
        s_2d_data.quad_buffer.size() * sizeof(Quad), BufferIOType::Dynamic);

    // Initialize quad vao
    layout.Clear();
    layout.Push(BufferLayoutType::Float3);  // position
    layout.Push(BufferLayoutType::Float4);  // color
    layout.Push(BufferLayoutType::Float2);  // texCoord
    layout.Push(BufferLayoutType::Int);     // texIndex
    layout.Push(BufferLayoutType::UInt);    // entity index
    s_2d_data.quad_vao = VertexArray::Create();
    s_2d_data.quad_vao->AddBufferLayout(layout);
    s_2d_data.quad_vao->BindVertexBuffer(*s_2d_data.quad_vbo, 0);
    s_2d_data.quad_vao->BindIndexBuffer(*s_2d_data.quad_ibo);

    // Initialize circle vbo
    s_2d_data.circle_vbo = VertexBuffer::Create(
        s_2d_data.circle_buffer.data(),
        s_2d_data.circle_buffer.size() * sizeof(Circle), BufferIOType::Dynamic);

    // Initialize circle vao
    layout.Clear();
    layout.Push(BufferLayoutType::Float3);  // world_pos
    layout.Push(BufferLayoutType::Float3);  // local_pos
    layout.Push(BufferLayoutType::Float4);  // color
    layout.Push(BufferLayoutType::Float);   // thickness
    layout.Push(BufferLayoutType::Float);   // fade
    layout.Push(BufferLayoutType::UInt);    // entity index
    s_2d_data.circle_vao = VertexArray::Create();
    s_2d_data.circle_vao->AddBufferLayout(layout);
    s_2d_data.circle_vao->BindVertexBuffer(*s_2d_data.circle_vbo, 0);
    s_2d_data.circle_vao->BindIndexBuffer(*s_2d_data.quad_ibo);

    s_2d_data.default_texture =
        Texture::Create(1, 1, 1, MultiSampleLevel::None, TextureType::Normal2D,
                        DataFormat::RGBA32F,
                        {TextureWrap::Repeat, TextureMinFilter::Linear,
                         TextureMagFilter::Linear, MipmapMode::None});
    const float color[4] = {1, 1, 1, 1};
    s_2d_data.default_texture->SetPixels(0, 0, 0, 1, 1, 1, color);

    s_2d_data.texture_slots[0] = s_2d_data.default_texture.get();

    auto& shaders = ShaderManager::Get();
    s_line_shader =
        shaders.LoadShader("line", "line.vert.glsl", "line.frag.glsl");
    s_texture_shader = shaders.LoadShader("texture quad", "texture.vert.glsl",
                                          "texture.frag.glsl");
    s_cirlce_shader =
        shaders.LoadShader("circle", "circle.vert.glsl", "circle.frag.glsl");

    Renderer::BindCamera(*s_line_shader);
    Renderer::BindCamera(*s_cirlce_shader);
    Renderer::BindCamera(*s_texture_shader);
}

void Renderer2D::Begin() {}

void Renderer2D::End()
{
    Flush();
    StartBatch();
}

void Renderer2D::Reset()
{
    s_2d_data.line_cnt = 0;
    s_2d_data.line_draw_call = 0;

    s_2d_data.quad_cnt = 0;
    s_2d_data.quad_draw_call = 0;

    s_2d_data.circie_cnt = 0;
    s_2d_data.circle_draw_call = 0;
}

std::string Renderer2D::GetDebugInfo()
{
    return fmt::format(
        "Line: total draw counts:{}, total draw calls:{}.\n"
        "Quad: total draw counts:{}, total draw calls:{}.\n"
        "Cirlce: total draw counts:{}, total draw calls:{}.\n",
        s_2d_data.line_cnt, s_2d_data.line_draw_call, s_2d_data.quad_cnt,
        s_2d_data.quad_draw_call, s_2d_data.circie_cnt,
        s_2d_data.circle_draw_call);
}

void Renderer2D::StartBatch()
{
    // Reset text
    SetTextOrigin(0, 0);
    s_2d_data.text_cursor.x = 0;
    s_2d_data.text_cursor.y = 0;

    StartLineBatch();
    StartQuadBatch();
    StartCircleBatch();
}

void Renderer2D::StartLineBatch()
{
    s_2d_data.line_vertex_cnt = 0;
    s_2d_data.line_buffer_ptr = s_2d_data.line_buffer.data();
}

void Renderer2D::StartQuadBatch()
{
    // Reset texture
    s_2d_data.texture_index = 1;

    // Reset quad
    s_2d_data.quad_index_cnt = 0;
    s_2d_data.quad_buffer_ptr = s_2d_data.quad_buffer.data();
}

void Renderer2D::StartCircleBatch()
{
    // Reset circle
    s_2d_data.circle_index_cnt = 0;
    s_2d_data.circle_buffer_ptr = s_2d_data.circle_buffer.data();
}

void Renderer2D::SetTextOrigin(int x, int y)
{
    s_2d_data.text_origin.x = x;
    s_2d_data.text_origin.y = y;
}

Vector2i Renderer2D::GetTextCursor()
{
    return s_2d_data.text_cursor + s_2d_data.text_origin;
}

void Renderer2D::Flush()
{
    FlushLines();
    FlushQuads();
    FlushCircles();
}

void Renderer2D::FlushLines()
{
    if (s_2d_data.line_vertex_cnt) {
        size_t offset =
            s_2d_data.line_buffer_ptr - s_2d_data.line_buffer.data();
        s_2d_data.line_vbo->UpdateData(s_2d_data.line_buffer.data(),
                                       sizeof(Line) * offset);
        Submit(*s_line_shader, *s_2d_data.line_vao, MeshTopology::Lines,
               s_2d_data.line_vertex_cnt, 0, false);
        s_2d_data.line_cnt += offset;
        ++s_2d_data.line_draw_call;
    }
}

void Renderer2D::FlushQuads()
{
    if (s_2d_data.quad_index_cnt) {
        size_t offset =
            s_2d_data.quad_buffer_ptr - s_2d_data.quad_buffer.data();
        s_2d_data.quad_vbo->UpdateData(s_2d_data.quad_buffer.data(),
                                       offset * sizeof(Quad));

        s_texture_shader->GetParam("u_textures[0]")
            ->SetAsTextures(s_2d_data.texture_slots.data(),
                            s_2d_data.texture_index);

        Submit(*s_texture_shader, *s_2d_data.quad_vao, MeshTopology::Triangles,
               s_2d_data.quad_index_cnt, 0);
        s_2d_data.quad_cnt += offset;
        ++s_2d_data.quad_draw_call;
    }
}

void Renderer2D::FlushCircles()
{
    if (s_2d_data.circle_index_cnt) {
        size_t offset =
            s_2d_data.circle_buffer_ptr - s_2d_data.circle_buffer.data();

        s_2d_data.circle_vbo->UpdateData(s_2d_data.circle_buffer.data(),
                                         offset * sizeof(Circle));

        Submit(*s_cirlce_shader, *s_2d_data.circle_vao, MeshTopology::Triangles,
               s_2d_data.circle_index_cnt, 0);
        s_2d_data.circie_cnt += offset;
        ++s_2d_data.circle_draw_call;
    }
}

void Renderer2D::NextLineBatch()
{
    FlushLines();
    StartLineBatch();
}

void Renderer2D::NextQuadBatch()
{
    FlushQuads();
    StartQuadBatch();
}

void Renderer2D::NextCircleBatch()
{
    FlushCircles();
    StartCircleBatch();
}

void Renderer2D::DrawLine(const Vector3f& start, const Vector3f& end,
                          const Vector4f& color, uint32_t entity_id)
{
    if (s_2d_data.line_vertex_cnt >= Renderer2DData::MAX_LINES_VERTICES) {
        NextLineBatch();
    }
    s_2d_data.line_buffer_ptr->vertices[0].pos = start;
    s_2d_data.line_buffer_ptr->vertices[0].color = color;
    s_2d_data.line_buffer_ptr->vertices[0].entity_id = entity_id;

    s_2d_data.line_buffer_ptr->vertices[1].pos = end;
    s_2d_data.line_buffer_ptr->vertices[1].color = color;
    s_2d_data.line_buffer_ptr->vertices[1].entity_id = entity_id;

    ++s_2d_data.line_buffer_ptr;
    s_2d_data.line_vertex_cnt += 2;
}

void Renderer2D::DrawQuad(const Vector3f& pos, const Quaternion& rot,
                          const Vector2f& scale, const Vector4f& color,
                          uint32_t entity_id)
{
    DrawQuad(glm::translate(Matrix4f(1.0f), pos) * glm::toMat4(rot) *
                 glm::scale(Matrix4f(1.0f), Vector3f(scale.x, scale.y, 1.0f)),
             color, entity_id);
}

void Renderer2D::DrawQuad(const Matrix4f& transform, const Vector4f& color,
                          uint32_t entity_id)
{
    if (s_2d_data.quad_index_cnt >= Renderer2DData::MAX_INDICES) {
        NextQuadBatch();
    }
    for (uint32_t i = 0; i < 4; ++i) {
        s_2d_data.quad_buffer_ptr->vertices[i].position =
            transform * QUAD_VERTEX_POS[i];
        s_2d_data.quad_buffer_ptr->vertices[i].color = color;
        s_2d_data.quad_buffer_ptr->vertices[i].uv.x = QUAD_UV[i].x;
        s_2d_data.quad_buffer_ptr->vertices[i].uv.y = QUAD_UV[i].y;
        s_2d_data.quad_buffer_ptr->vertices[i].tex_id = 0;
        s_2d_data.quad_buffer_ptr->vertices[i].entity_id = entity_id;
    }
    ++s_2d_data.quad_buffer_ptr;
    s_2d_data.quad_index_cnt += 6;
}

void Renderer2D::DrawTexture(const Texture& texture,
                             const std::array<Vector2f, 2>& uv,
                             const Vector3f& pos, const Quaternion& rot,
                             const Vector2f& scale, const Vector4f& color,
                             uint32_t entity_id)
{
    DrawTexture(
        texture, uv,
        glm::translate(Matrix4f(1.0f), pos) * glm::toMat4(rot) *
            glm::scale(Matrix4f(1.0f), Vector3f(scale.x, scale.y, 1.0f)),
        color, entity_id);
}

void Renderer2D::DrawTexture(const Texture& texture,
                             const std::array<Vector2f, 2>& uv,
                             const Matrix4f& transform, const Vector4f& color,
                             uint32_t entity_id)
{
    if (s_2d_data.quad_index_cnt >= Renderer2DData::MAX_INDICES) {
        NextQuadBatch();
    }

    uint32_t textureIndex = 0;
    for (uint32_t i = 1; i < s_2d_data.texture_index; ++i) {
        if (*s_2d_data.texture_slots[i] == texture) {
            textureIndex = i;
            break;
        }
    }

    if (textureIndex == 0) {
        if (s_2d_data.texture_index >= Renderer2DData::MAX_TEXTURE_SLOTS) {
            NextQuadBatch();
        }
        textureIndex = s_2d_data.texture_index;
        s_2d_data.texture_slots[s_2d_data.texture_index++] = &texture;
    }

    for (uint32_t i = 0; i < 4; ++i) {
        s_2d_data.quad_buffer_ptr->vertices[i].position =
            transform * QUAD_VERTEX_POS[i];
        s_2d_data.quad_buffer_ptr->vertices[i].color = color;
        s_2d_data.quad_buffer_ptr->vertices[i].uv.x = uv[QUAD_UV[i].x].x;
        s_2d_data.quad_buffer_ptr->vertices[i].uv.y = uv[QUAD_UV[i].y].y;
        s_2d_data.quad_buffer_ptr->vertices[i].tex_id = textureIndex;
        s_2d_data.quad_buffer_ptr->vertices[i].entity_id = entity_id;
    }
    ++s_2d_data.quad_buffer_ptr;
    s_2d_data.quad_index_cnt += 6;
}

void Renderer2D::DrawTexture(const Texture& texture, const Vector3f& pos,
                             const Quaternion& rot, const Vector2f& scale,
                             const Vector4f& color, uint32_t entity_id)
{
    DrawTexture(
        texture, {Vector2f(0), Vector2f(1)},
        glm::translate(Matrix4f(1.0f), pos) * glm::toMat4(rot) *
            glm::scale(Matrix4f(1.0f), Vector3f(scale.x, scale.y, 1.0f)),
        color, entity_id);
}

void Renderer2D::DrawTexture(const Texture& texture, const Matrix4f& transform,
                             const Vector4f& color, uint32_t entity_id)
{
    DrawTexture(texture, {Vector2f(0), Vector2f(1)}, transform, color,
                entity_id);
}

void Renderer2D::DrawBillboard(const Texture& texture,
                               const std::array<Vector2f, 2>& uv,
                               const Vector3f& pos, const Vector2f& scale,
                               const Vector4f& color, uint32_t entity_id)
{
    DrawTexture(
        texture, uv,
        glm::translate(Matrix4f(1.0f), pos) *
            Matrix4f(glm::transpose(Matrix3f(s_data.camera_data.view))) *
            glm::scale(Matrix4f(1.0f), Vector3f(scale.x, scale.y, 1.0f)),
        color, entity_id);
}

void Renderer2D::DrawBillboard(const Texture& texture, const Vector3f& pos,
                               const Vector2f& scale, const Vector4f& color,
                               uint32_t entity_id)
{
    DrawTexture(
        texture,
        glm::translate(Matrix4f(1.0f), pos) *
            Matrix4f(glm::transpose(Matrix3f(s_data.camera_data.view))) *
            glm::scale(Matrix4f(1.0f), Vector3f(scale.x, scale.y, 1.0f)),
        color, entity_id);
}

void Renderer2D::DrawText(const Font& font, const std::string& text,
                          const Matrix4f& transform, const Vector4f& color,
                          uint32_t entity_id)
{
    Matrix4f t =
        glm::translate(Matrix4f(1.0f), Vector3f(s_2d_data.text_origin.x,
                                                s_2d_data.text_origin.y, 0)) *
        transform;
    std::u32string u32str = String::ConvertToUTF32(text);
    for (const auto c : u32str) {
        if (c == '\n') {
            s_2d_data.text_cursor.x = 0;
            s_2d_data.text_cursor.y -= font.GetPixelHeight();
            continue;
        }
        else if (c <= 128 && std::iscntrl(c)) {
            continue;
        }
        const Character& ch = font.GetCharacter(c);
        Matrix4f offset =
            glm::translate(
                Matrix4f(1.0f),
                Vector3f(
                    s_2d_data.text_cursor.x + ch.bearing.x + ch.size.x * 0.5f,
                    s_2d_data.text_cursor.y + ch.bearing.y - ch.size.y * 0.5f,
                    0)) *
            glm::scale(Matrix4f(1.0f), Vector3f(ch.size.x, ch.size.y, 1.0f));
        DrawTexture(*ch.glyph, ch.uv, t * offset, color, entity_id);
        s_2d_data.text_cursor.x += ch.advance;
    }
}

void Renderer2D::DrawCircle(const Vector3f& pos, const Vector2f& scale,
                            const Vector4f& color, float thickness, float fade,
                            uint32_t entity_id)
{
    DrawCircle(glm::translate(Matrix4f(1.0f), pos) *
                   glm::scale(Matrix4f(1.0f), Vector3f(scale.x, scale.y, 1.0f)),
               color, thickness, fade, entity_id);
}

void Renderer2D::DrawCircle(const Matrix4f& transform, const Vector4f& color,
                            float thickness, float fade, uint32_t entity_id)
{
    if (s_2d_data.circle_index_cnt >= Renderer2DData::MAX_INDICES) {
        NextCircleBatch();
    }
    for (size_t i = 0; i < 4; ++i) {
        s_2d_data.circle_buffer_ptr->vertices[i].world_pos =
            transform * QUAD_VERTEX_POS[i];
        s_2d_data.circle_buffer_ptr->vertices[i].local_pos =
            QUAD_VERTEX_POS[i] * 2.f;
        s_2d_data.circle_buffer_ptr->vertices[i].color = color;
        s_2d_data.circle_buffer_ptr->vertices[i].thickness = thickness;
        s_2d_data.circle_buffer_ptr->vertices[i].fade = fade;
        s_2d_data.circle_buffer_ptr->vertices[i].entity_id = entity_id;
    }
    ++s_2d_data.circle_buffer_ptr;
    s_2d_data.circle_index_cnt += 6;
}
}  // namespace SD
