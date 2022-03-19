#include "System/SpriteRenderSystem.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Graphics/Font.hpp"

#include "Asset/AssetStorage.hpp"
#include "Asset/TextureAsset.hpp"
#include "Asset/FontAsset.hpp"

namespace SD {

SpriteRenderSystem::SpriteRenderSystem() : System("SpriteRenderSystem") {}

void SpriteRenderSystem::OnPush() {}

void SpriteRenderSystem::OnPop() {}

void SpriteRenderSystem::OnTick(float dt)
{
    auto anim_view = scene->view<SpriteAnimationComponent>();
    anim_view.each([&](SpriteAnimationComponent &anim_comp) {
        if (!anim_comp.animations.empty()) {
            anim_comp.animator.Tick(dt);
        }
    });
}

struct SpriteDrawData {
    Texture *texture;
    std::array<glm::vec2, 2> uvs;
    glm::vec3 pos;
    glm::quat rot;
    glm::vec2 size;
    uint32_t entity_id;
    int priority;
};
void SpriteRenderSystem::OnRender()
{
    int index[] = {0, 1};
    RenderOperation op;
    op.depth_mask = false;
    Renderer::BeginRenderSubpass(RenderSubpassInfo{index, 2, op});
    Renderer2D::Begin(*scene->GetCamera());

    std::vector<SpriteDrawData> datas;
    auto sprite_view = scene->view<SpriteComponent, TransformComponent>();
    sprite_view.each([&datas](entt::entity entity_id,

                              const SpriteComponent &sprite_comp,
                              const TransformComponent &transform_comp) {
        uint32_t id = static_cast<uint32_t>(entity_id);
        auto &frame = sprite_comp.frame;
        if (AssetStorage::Get().Exists<TextureAsset>(frame.texture_id)) {
            auto texture = AssetStorage::Get()
                               .GetAsset<TextureAsset>(frame.texture_id)
                               ->GetTexture();
            datas.push_back({texture, frame.uvs,
                             transform_comp.GetWorldPosition(),
                             transform_comp.GetWorldRotation(), frame.size, id,
                             frame.priority});
        }
    });
    auto anim_view =
        scene->view<SpriteAnimationComponent, TransformComponent>();
    anim_view.each([&datas](entt::entity entity_id,
                            const SpriteAnimationComponent &anim_comp,
                            const TransformComponent &transform_comp) {
        uint32_t id = static_cast<uint32_t>(entity_id);
        auto anim = anim_comp.animator.GetAnimation();
        if (anim) {
            if (anim->GetFrameSize()) {
                auto &frame = anim->GetFrame();
                if (AssetStorage::Get().Exists<TextureAsset>(
                        frame.texture_id)) {
                    auto texture = AssetStorage::Get()
                                       .GetAsset<TextureAsset>(frame.texture_id)
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
                                    data.size, glm::vec4(1.0f), data.entity_id);
    }

    auto textView = scene->view<TransformComponent, TextComponent>();

    textView.each([](entt::entity entity_id,
                     const TransformComponent &transformComp,
                     const TextComponent &textComp) {
        Renderer2D::SetTextOrigin(0, 0);
        if (AssetStorage::Get().Exists<FontAsset>(textComp.font_id)) {
            auto font = AssetStorage::Get()
                            .GetAsset<FontAsset>(textComp.font_id)
                            ->GetFont();
            Renderer2D::DrawText(
                *font, textComp.text,
                transformComp.GetWorldTransform().GetMatrix(), textComp.color,
                static_cast<uint32_t>(entity_id));
        }
    });
    Renderer2D::End();

    Renderer::EndRenderSubpass();
}

}  // namespace SD
