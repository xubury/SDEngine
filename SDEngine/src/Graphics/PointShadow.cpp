#include "Graphics/PointShadow.hpp"

namespace SD {

PointShadow::PointShadow(int32_t width, int32_t height)
{
    m_far_z = 25.f;
    m_near_z = 1.f;
    m_framebuffer = Framebuffer::Create();
    m_shadow_map = Texture::Create(width, height, 0, MultiSampleLevel::None,
                                   TextureType::Cube, DataFormat::Depth24);
    m_framebuffer->Attach(*m_shadow_map, 0, 0);
}

}  // namespace SD
