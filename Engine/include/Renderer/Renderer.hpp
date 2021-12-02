#ifndef SD_RENDERER_HPP
#define SD_RENDERER_HPP

#include "Renderer/RenderTarget.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Mesh.hpp"
#include "Renderer/Camera.hpp"
#include "Asset/Font.hpp"

namespace SD {

class AssetManager;

class Scene;

struct QuadVertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 uv;
    float tex_id;
    QuadVertex() : position(0.f), color(0.f), uv(0.f), tex_id(0.f) {}
};

struct Quad {
    std::array<QuadVertex, 4> vertices;
    glm::vec3 GetCenter() const {
        return (vertices[0].position + vertices[1].position +
                vertices[2].position + vertices[3].position) /
               glm::vec3(4);
    }
};

struct CameraData {
    glm::mat4 view_projection;
    glm::vec3 view_pos;
};

struct Renderer2DData {
    static const uint32_t MAX_QUADS = 20000;
    static const uint32_t MAX_VERTICES = MAX_QUADS * 4;
    static const uint32_t MAX_INDICES = MAX_QUADS * 6;
    static const uint32_t MAX_TEXTURE_SLOTS = 32;

    Ref<VertexArray> quad_vao;
    Ref<Shader> sprite_shader;

    size_t quad_index_cnt = 0;
    std::array<Quad, MAX_QUADS> quad_buffers;
    Quad *quad_buffers_ptr = nullptr;

    std::array<glm::vec4, 4> quad_vertex_pos;
    std::array<glm::vec2, 2> quad_uv;

    uint32_t texture_index = 1;
    std::array<Ref<Texture>, MAX_TEXTURE_SLOTS> texture_slots;

    glm::vec2 textOrigin;
    glm::vec2 text_cursor;
};

class SD_RENDERER_API Renderer {
   public:
    Renderer(int msaa);
    ~Renderer() { SD_CORE_TRACE("Deleting Renderer"); }
    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    void SetRenderTarget(RenderTarget &target);

    void UpdateShader(Shader &shader, Camera &camera);

    void Submit(const VertexArray &vao, MeshTopology topology, size_t count,
                size_t offset);

    void DrawMesh(const Mesh &mesh);

    void StartBatch();
    void Flush();
    void NextBatch();

    void BeginScene(Camera &camera);
    void EndScene();

    void SetTextOrigin(float x, float y);

    void DrawQuad(const glm::mat4 &transform, const glm::vec4 &color);

    void DrawTexture(const Ref<Texture> &texture, const glm::mat4 &transform,
                     const std::array<glm::vec2, 2> &texCoords,
                     const glm::vec4 &color = glm::vec4(1.0f));
    void DrawTexture(const Ref<Texture> &texture, const glm::mat4 &transform,
                     const glm::vec4 &color = glm::vec4(1.0f));

    void DrawBillboard(const Ref<Texture> &texture, const glm::vec3 &pos,
                       const glm::vec2 &scale,
                       const glm::vec4 &color = glm::vec4(1.0f));

    void DrawText(Font &font, const std::string &text, uint8_t pixleSize,
                  const glm::mat4 &transform,
                  const glm::vec4 &color = glm::vec4(1.0f));

    void SetScene(Scene *scene);
    Scene *GetScene();

    void SetCamera(Camera *camera);
    Camera *GetCamera();

   private:
    void InitRenderer2D();

    Scene *m_scene;
    Camera *m_camera;

    Ref<UniformBuffer> m_camera_UBO;

    Renderer2DData m_2d_data;
};

}  // namespace SD

#endif /* SD_RENDERER_HPP */
