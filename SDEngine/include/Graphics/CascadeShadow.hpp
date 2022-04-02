#ifndef SD_CASCADE_SHADOW_HPP
#define SD_CASCADE_SHADOW_HPP

#include "Graphics/Framebuffer.hpp"
#include "Graphics/Camera.hpp"
#include "Utility/Transform.hpp"
#include <vector>

namespace SD {

class SD_GRAPHICS_API CascadeShadow {
   public:
    CascadeShadow();
    void CreateShadowMap();
    void DestroyShadowMap();
    const std::vector<Matrix4f> &GetLevelProjectionView() const
    {
        return m_projection_views;
    }

    void SetCascadePlanes(const std::vector<float> &planes)
    {
        m_cascade_planes = planes;
    }

    void SetNumOfCascades(int32_t num_of_cascades);
    const std::vector<float> &GetCascadePlanes() const
    {
        return m_cascade_planes;
    }

    void ComputeCascadeLightMatrix(const Transform &transform,
                                   const Camera &camera);

    Texture *GetCascadeMap() const { return m_cascade_map.get(); }
    Framebuffer *GetCascadeFramebuffer() const { return m_cascade_fb.get(); }

    SERIALIZE(m_cascade_planes)
   private:
    Ref<Framebuffer> m_cascade_fb;
    Ref<Texture> m_cascade_map;
    std::vector<Matrix4f> m_projection_views;
    std::vector<float> m_cascade_planes;
};
}  // namespace SD

#endif
