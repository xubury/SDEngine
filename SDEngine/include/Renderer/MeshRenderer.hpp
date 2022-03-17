#ifndef SD_MESH_RENDERER_HPP
#define SD_MESH_RENDERER_HPP

#include "Renderer/Export.hpp"
#include "Renderer/Renderer.hpp"

#include "Graphics/Material.hpp"
namespace SD {

struct SD_RENDERER_API ShadowData {
    glm::mat4 projection_view[16];
};

class SD_RENDERER_API MeshRenderer : protected Renderer {
   public:
    static void Init();
    static void Begin(Shader &shader, Camera &camera);
    static void Begin(Light &light, const Transform &light_trans,
                      Camera &camera, Shader &shader);
    static void End();

    static void DrawMesh(const Shader &shader, const Mesh &mesh);
    static void SetMaterial(Shader &shader, const Material &material);

   private:
    static void SetupShaderUBO(Shader &shader);
    static Ref<UniformBuffer> m_shadow_UBO;

    static Ref<VertexArray> m_mesh_vao;
};

}  // namespace SD
#endif
