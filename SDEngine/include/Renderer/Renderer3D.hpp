#ifndef SD_RENDERER_3D_HPP
#define SD_RENDERER_3D_HPP

#include "Graphics/CascadeShadow.hpp"
#include "Renderer/Export.hpp"
#include "Renderer/Renderer.hpp"

#include "Graphics/Material.hpp"
namespace SD {

struct SD_RENDERER_API ShadowData {
    glm::mat4 projection_view[16];
};

class SD_RENDERER_API Renderer3D : protected Renderer {
   public:
    static void Init();
    static void SetShadowCaster(Shader &shader, CascadeShadow &shadow);

    static void DrawMesh(const Shader &shader, const Mesh &mesh);
    static void SetMaterial(Shader &shader, const Material &material);

   private:
    static Ref<UniformBuffer> m_shadow_UBO;
    static Ref<VertexArray> m_mesh_vao;
};

}  // namespace SD

#endif
