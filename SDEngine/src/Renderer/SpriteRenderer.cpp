#include "Renderer/SpriteRenderer.hpp"
#include "Renderer/Renderer2D.hpp"
#include "ECS/Component.hpp"

#include "Asset/AssetStorage.hpp"
#include "Asset/TextureAsset.hpp"
#include "Asset/FontAsset.hpp"

namespace SD {

struct SpriteDrawData {
    Texture *texture;
    std::array<Vector2f, 2> uvs;
    Vector3f pos;
    Quaternion rot;
    Vector2f size;
    uint32_t entity_id;
    int priority;
};

void SpriteRenderer::RenderScene(const Scene &scene)
{
    auto &storage = AssetStorage::Get();
    int index[] = {0, 1};
    RenderOperation op;
    op.depth_mask = false;
    Renderer::BeginRenderSubpass(RenderSubpassInfo{index, 2, op});
    Renderer2D::Begin();

    std::vector<SpriteDrawData> datas;
    auto sprite_view = scene.view<SpriteComponent, TransformComponent>();
    sprite_view.each([&](entt::entity entity_id,
                         const SpriteComponent &sprite_comp,
                         const TransformComponent &transform_comp) {
        uint32_t id = static_cast<uint32_t>(entity_id);
        auto &frame = sprite_comp.frame;
        if (storage.Exists<TextureAsset>(frame.texture_id)) {
            auto texture =
                storage.GetAsset<TextureAsset>(frame.texture_id)->GetTexture();
            datas.push_back({texture, frame.uvs,
                             transform_comp.GetWorldPosition(),
                             transform_comp.GetWorldRotation(), frame.size, id,
                             frame.priority});
        }
    });
    auto anim_view = scene.view<SpriteAnimationComponent, TransformComponent>();
    anim_view.each([&](entt::entity entity_id,
                       const SpriteAnimationComponent &anim_comp,
                       const TransformComponent &transform_comp) {
        uint32_t id = static_cast<uint32_t>(entity_id);
        auto anim = anim_comp.animator.GetAnimation();
        if (anim) {
            if (anim->GetFrameSize()) {
                auto &frame = anim->GetFrame();
                if (storage.Exists<TextureAsset>(frame.texture_id)) {
                    auto texture =
                        storage.GetAsset<TextureAsset>(frame.texture_id)
                            ->GetTexture();
                    datas.push_back({texture, frame.uvs,
                                     transform_comp.GetWorldPosition(),
                                     transform_comp.GetWorldRotation(),
                                     frame.size, id, frame.priority});
                }
            }
        }
    });

    std::sort(datas.begin(), datas.end(), [](const auto &lhs, const auto &rhs) {
        auto l_z = lhs.pos.z;
        auto r_z = rhs.pos.z;
        auto l_p = lhs.priority;
        auto r_p = rhs.priority;
        if (l_z < r_z) {
            return true;
        }
        else if (l_z > r_z) {
            return false;
        }
        else {
            return l_p < r_p;
        }
    });
    for (const auto &data : datas) {
        Renderer2D::DrawTexture(*data.texture, data.uvs, data.pos, data.rot,
                                data.size, Vector4f(1.0f), data.entity_id);
    }

    auto textView = scene.view<TransformComponent, TextComponent>();

    textView.each([&](entt::entity entity_id,
                      const TransformComponent &transformComp,
                      const TextComponent &textComp) {
        Renderer2D::SetTextOrigin(0, 0);
        if (storage.Exists<FontAsset>(textComp.font_id)) {
            auto font =
                storage.GetAsset<FontAsset>(textComp.font_id)->GetFont();
            Renderer2D::DrawText(*font, textComp.text,
                                 transformComp.GetWorldTransform().GetMatrix(),
                                 textComp.color,
                                 static_cast<uint32_t>(entity_id));
        }
    });
    Renderer2D::End();

    Renderer::EndRenderSubpass();
}

}  // namespace SD
