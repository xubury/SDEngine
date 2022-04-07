#ifndef SD_RENDERER_3D_HPP
#define SD_RENDERER_3D_HPP

#include "Graphics/CascadeShadow.hpp"
#include "Renderer/Export.hpp"
#include "Renderer/Renderer.hpp"

#include "Graphics/Material.hpp"
namespace SD {

struct SD_RENDERER_API ShadowData {
    Matrix4f projection_view[16];
};

class SD_RENDERER_API Renderer3D : protected Renderer {
   public:
    static void Init();
    static void Reset();
    static std::string GetDebugInfo();
    static void SetCascadeShadow(const CascadeShadow &shadow);
    static void BindCascadeShadow(Shader &shader);

    static void DrawMesh(const Shader &shader, const Mesh &mesh);
    static void SetMaterial(Shader &shader, const Material &material);
};

}  // namespace SD

#endif
