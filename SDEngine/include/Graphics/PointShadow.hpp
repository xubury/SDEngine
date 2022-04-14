#ifndef SD_POINT_SHADOW_HPP
#define SD_POINT_SHADOW_HPP

#include "Graphics/Framebuffer.hpp"
#include "Graphics/Export.hpp"
#include "Utility/Math.hpp"
#include "Utility/Serialize.hpp"

namespace SD {

class SD_GRAPHICS_API PointShadow {
   public:
    PointShadow();
    void CreateShadowMap(int32_t width = 4096, int32_t height = 4096);
    void DestroyShadowMap();

    Texture *GetShadowMap() { return m_shadow_map.get(); }
    Framebuffer *GetShadowTarget() { return m_framebuffer.get(); }

    const std::array<Matrix4f, 6> &GetProjectionMatrix(
        const Vector3f &light_pos)
    {
        if (m_last_pos != light_pos) {
            m_outdated = true;
            m_last_pos = light_pos;
        }
        if (m_outdated) {
            ComputeShadowMatirx();
        }
        return m_shadow_matrix;
    }

    float GetNearZ() const { return m_near_z; }
    float GetFarZ() const { return m_far_z; }

    void SetNearZ(float near_z)
    {
        m_near_z = near_z;
        m_outdated = true;
    }
    void SetFarZ(float far_z)
    {
        m_far_z = far_z;
        m_outdated = true;
    }

    SERIALIZE(m_far_z, m_near_z)
   private:
    void ComputeShadowMatirx()
    {
        m_projection =
            glm::perspective(glm::radians(90.f),
                             static_cast<float>(m_shadow_map->GetWidth()) /
                                 m_shadow_map->GetHeight(),
                             m_near_z, m_far_z);
        m_shadow_matrix[0] =
            m_projection * glm::lookAt(m_last_pos,
                                       m_last_pos + glm::vec3(1.0f, 0.0f, 0.0f),
                                       glm::vec3(0.0f, -1.0f, 0.0f));
        m_shadow_matrix[1] =
            m_projection *
            glm::lookAt(m_last_pos, m_last_pos + glm::vec3(-1.0f, 0.0f, 0.0f),
                        glm::vec3(0.0f, -1.0f, 0.0f));
        m_shadow_matrix[2] =
            m_projection * glm::lookAt(m_last_pos,
                                       m_last_pos + glm::vec3(0.0f, 1.0f, 0.0f),
                                       glm::vec3(0.0f, 0.0f, 1.0f));
        m_shadow_matrix[3] =
            m_projection *
            glm::lookAt(m_last_pos, m_last_pos + glm::vec3(0.0f, -1.0f, 0.0f),
                        glm::vec3(0.0f, 0.0f, -1.0f));
        m_shadow_matrix[4] =
            m_projection * glm::lookAt(m_last_pos,
                                       m_last_pos + glm::vec3(0.0f, 0.0f, 1.0f),
                                       glm::vec3(0.0f, -1.0f, 0.0f));
        m_shadow_matrix[5] =
            m_projection *
            glm::lookAt(m_last_pos, m_last_pos + glm::vec3(0.0f, 0.0f, -1.0f),
                        glm::vec3(0.0f, -1.0f, 0.0f));
        m_outdated = false;
    }
    Ref<Framebuffer> m_framebuffer;
    Ref<Texture> m_shadow_map;
    Matrix4f m_projection;
    std::array<Matrix4f, 6> m_shadow_matrix;

    float m_far_z;
    float m_near_z;

    Vector3f m_last_pos;
    bool m_outdated{true};
};

}  // namespace SD

#endif
