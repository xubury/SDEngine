#ifndef SD_CASCADE_SHADOW_HPP
#define SD_CASCADE_SHADOW_HPP

#include "Graphics/Framebuffer.hpp"
#include "Graphics/Camera.hpp"
#include "Utility/Transform.hpp"
#include <vector>
#include <glm/glm.hpp>

namespace SD {

class SD_GRAPHICS_API CascadeShadow {
   public:
    CascadeShadow();
    void CreateShadowMap();
    void DestroyShadowMap();
    const std::vector<glm::mat4> &GetLevelProjectionView() const
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

    Framebuffer *GetCascadeMap() const { return m_cascade_map.get(); }

    SERIALIZE(m_cascade_planes)
   private:
    Ref<Framebuffer> m_cascade_map;
    std::vector<glm::mat4> m_projection_views;
    std::vector<float> m_cascade_planes;
};
}  // namespace SD

#endif
