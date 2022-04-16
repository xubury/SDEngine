#include "GridRenderer.hpp"
#include "ECS/Component.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Graphics/Texture.hpp"
#include "TileBrush.hpp"

namespace SD {

struct GridRenderData {
    Ref<Texture> grid_texture;
};
static GridRenderData s_data;

const int grid_texture_size = 100;
const int line_width = 5;

void GridRenderer::Init()
{
    s_data.grid_texture = Texture::Create(
        grid_texture_size, grid_texture_size, 1, MultiSampleLevel::None,
        TextureType::Normal2D, DataFormat::RGBA8, {TextureWrap::Repeat});
    size_t pixel_size =
        4 * s_data.grid_texture->GetWidth() * s_data.grid_texture->GetHeight();
    void *data = malloc(pixel_size);
    memset(data, 0xff, pixel_size);
    s_data.grid_texture->SetPixels(0, 0, 0, s_data.grid_texture->GetWidth(),
                                   s_data.grid_texture->GetHeight(), 1, data);
    memset(data, 0x00, pixel_size);
    s_data.grid_texture->SetPixels(
        line_width, line_width, 0,
        s_data.grid_texture->GetWidth() - line_width * 2,
        s_data.grid_texture->GetHeight() - line_width * 2, 1, data);

    free(data);
}

void GridRenderer::Render(const Scene &scene, const Camera &camera,
                          const SpriteFrame &frame, const TileBrush &brush)
{
    const int index = 0;
    RenderOperation op;
    op.depth_test = false;
    Renderer::BeginRenderSubpass(RenderSubpassInfo{&index, 1, op});
    Renderer2D::Begin();

    if (brush.is_painting) {
        Vector3f world = brush.GetSelectdPos();
        auto &resource = scene.GetResourceRegistry().GetTextureCache();
        if (resource.Contains(frame.texture_id)) {
            Renderer2D::DrawTexture(*resource.Handle(frame.texture_id),
                                    frame.uvs, world, Quaternion(), frame.size);
        }
        // draw overlay
        Renderer2D::DrawQuad(world, Quaternion(), frame.size, brush.color);
    }

    // draw brush & outline
    const Vector2i &tile_size = brush.tile_size;
    auto &render_pass = Renderer::GetCurrentRenderPass();
    int render_width = render_pass.viewport_width;
    int render_height = render_pass.viewport_height;
    const Vector2i tile_cnt(
        std::ceil(static_cast<float>(render_width) / tile_size.x) + 1,
        std::ceil(static_cast<float>(render_height) / tile_size.y) + 1);
    const Vector2f tex_size = tile_cnt * tile_size;

    const Vector3f cam_pos = camera.GetWorldPosition();
    Vector2f uv_origin(cam_pos.x / tile_size.x, -cam_pos.y / tile_size.y);
    const Vector3f outline_pos(
        tex_size.x / 2.f + cam_pos.x - tile_size.x / 2.f -
            tile_size.x * std::floor(tile_cnt.x / 2.f),
        -tex_size.y / 2.f + cam_pos.y + tile_size.y / 2.f +
            tile_size.y * std::floor(tile_cnt.y / 2.f),
        0);
    Renderer2D::DrawTexture(
        *s_data.grid_texture, {uv_origin, Vector2f(tile_cnt) + uv_origin},
        outline_pos, Quaternion(), tex_size, Vector4f(1, 1, 1, 0.7));
    Renderer2D::End();
    Renderer::EndRenderSubpass();
}
}  // namespace SD
