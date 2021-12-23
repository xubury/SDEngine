#ifndef SD_RENDERER_HPP
#define SD_RENDERER_HPP

#include "Graphics/Graphics.hpp"
#include "Graphics/RenderTarget.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Shader.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/Font.hpp"

namespace SD {

class AssetManager;

class Scene;

struct SD_RENDERER_API LineVertex {
    glm::vec3 pos;
    glm::vec4 color;
};

struct SD_RENDERER_API Line {
    std::array<LineVertex, 2> vertices;
};

struct SD_RENDERER_API QuadVertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 uv;
    float tex_id;
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
};

struct SD_RENDERER_API Circle {
    std::array<CircleVertex, 4> vertices;
};

struct SD_RENDERER_API CameraData {
    glm::mat4 projection;
    glm::mat4 view;
};

struct SD_RENDERER_API Renderer2DData {
    static const uint32_t MAX_QUADS = 20000;
    static const uint32_t MAX_LINES = 20000;
    static const uint32_t MAX_LINES_VERTICES = MAX_LINES * 2;
    static const uint32_t MAX_INDICES = MAX_QUADS * 6;
    static const uint32_t MAX_TEXTURE_SLOTS = 32;

    Ref<VertexArray> line_vao;
    size_t line_vertex_cnt = 0;
    std::array<Line, MAX_LINES> line_buffer;
    Line *line_buffer_ptr = nullptr;

    Ref<VertexArray> quad_vao;
    size_t quad_index_cnt = 0;
    std::array<Quad, MAX_QUADS> quad_buffer;
    Quad *quad_buffer_ptr = nullptr;

    uint32_t texture_index = 1;
    std::array<Ref<Texture>, MAX_TEXTURE_SLOTS> texture_slots;

    Ref<VertexArray> circle_vao;
    size_t circle_index_cnt = 0;
    std::array<Circle, MAX_QUADS> circle_buffer;
    Circle *circle_buffer_ptr = nullptr;

    glm::ivec2 text_origin;
    glm::ivec2 text_cursor;
};

class SD_RENDERER_API Renderer {
   public:
    Renderer(Device *device);
    ~Renderer();
    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    void Submit(const VertexArray &vao, MeshTopology topology, size_t count,
                size_t offset);

    void DrawMesh(const Mesh &mesh);

    void StartBatch();
    void Flush();

    void Begin(Camera &camera);
    void Begin(Shader &shader, Camera &camera);
    void End();

    void SetTextOrigin(int x, int y);
    glm::ivec2 GetTextCursor() const;

    void DrawLine(const glm::vec3 &start, const glm::vec3 &end,
                  const glm::vec4 &color);

    void DrawQuad(const glm::vec3 &pos, const glm::quat &rot,
                  const glm::vec2 &scale, const glm::vec4 &color);
    void DrawQuad(const glm::mat4 &transform, const glm::vec4 &color);

    void DrawTexture(const Ref<Texture> &texture,
                     const std::array<glm::vec2, 2> &uv, const glm::vec3 &pos,
                     const glm::quat &rot, const glm::vec2 &scale,
                     const glm::vec4 &color = glm::vec4(1.0f));
    void DrawTexture(const Ref<Texture> &texture,
                     const std::array<glm::vec2, 2> &uv,
                     const glm::mat4 &transform,
                     const glm::vec4 &color = glm::vec4(1.0f));

    void DrawTexture(const Ref<Texture> &texture, const glm::vec3 &pos,
                     const glm::quat &rot, const glm::vec2 &scale,
                     const glm::vec4 &color = glm::vec4(1.0f));
    void DrawTexture(const Ref<Texture> &texture, const glm::mat4 &transform,
                     const glm::vec4 &color = glm::vec4(1.0f));

    void DrawBillboard(const Ref<Texture> &texture,
                       const std::array<glm::vec2, 2> &uv, const glm::vec3 &pos,
                       const glm::vec2 &scale,
                       const glm::vec4 &color = glm::vec4(1.0f));
    void DrawBillboard(const Ref<Texture> &texture, const glm::vec3 &pos,
                       const glm::vec2 &scale,
                       const glm::vec4 &color = glm::vec4(1.0f));

    void DrawText(const Font &font, const std::string &text, uint8_t pixleSize,
                  const glm::mat4 &transform,
                  const glm::vec4 &color = glm::vec4(1.0f));

    void DrawCircle(const glm::vec3 &pos, const glm::vec2 &scale,
                    const glm::vec4 &color, float thickness, float fade);
    void DrawCircle(const glm::mat4 &transform, const glm::vec4 &color,
                    float thickness, float fade);

    void RenderToScreen();

    Framebuffer *GetFramebuffer() { return m_target.GetFramebuffer(); }

    RenderTarget &GetDefaultTarget() { return m_target; }

   private:
    void InitRenderer2D();

    void StartLineBatch();
    void StartQuadBatch();
    void StartCircleBatch();

    void FlushLines();
    void FlushQuads();
    void FlushCircles();

    void NextLineBatch();
    void NextQuadBatch();
    void NextCircleBatch();

    void SetupShaderUBO(Shader &shader);

    Device *m_device;

    Ref<UniformBuffer> m_camera_UBO;

    Renderer2DData m_data;

    RenderTarget m_target;

    Ref<Shader> m_line_shader;
    Ref<Shader> m_circle_shader;
    Ref<Shader> m_sprite_shader;

    CameraData m_camera_data;
};

}  // namespace SD

#endif /* SD_RENDERER_HPP */
