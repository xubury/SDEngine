#include "Graphics/PointShadow.hpp"

namespace SD {

const int shadow_map_width = 4086;
const int shadow_map_height = 4086;

PointShadow::PointShadow()
{
    m_far_z = 25.f;
    m_near_z = 1.f;
}

void PointShadow::CreateShadowMap()
{
    m_framebuffer = Framebuffer::Create();
    m_shadow_map = Texture::Create(shadow_map_width, shadow_map_height, 0,
                                   MultiSampleLevel::None, TextureType::Cube,
                                   DataFormat::Depth24);
    m_framebuffer->Attach(*m_shadow_map, 0, 0);
}

void PointShadow::DestroyShadowMap()
{
    m_framebuffer.reset();
    m_shadow_map.reset();
}

}  // namespace SD
