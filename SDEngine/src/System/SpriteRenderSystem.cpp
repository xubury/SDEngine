#include "System/SpriteRenderSystem.hpp"
#include "Renderer/Font.hpp"
#include "Renderer/Bitmap.hpp"
#include "Renderer/Sprite.hpp"

namespace SD {

SpriteRenderSystem::SpriteRenderSystem() : System("SpriteRenderSystem") {}

void SpriteRenderSystem::OnPush() {}

void SpriteRenderSystem::OnPop() {}

struct SpriteDrawData {
    Texture *texture;
    std::array<glm::vec2, 2> uvs;
    glm::vec3 pos;
    glm::quat rot;
    glm::vec2 size;
    uint32_t entity_id;
    int priority;
};
void SpriteRenderSystem::OnRender() {
    device->SetFramebuffer(renderer->GetFramebuffer());
    device->SetDepthMask(false);
    renderer->Begin(*scene->GetCamera());
    std::vector<SpriteDrawData> datas;
    auto sprite_view =
        scene->view<PriorityComponent, SpriteComponent, TransformComponent>();
    sprite_view.each([this, &datas](entt::entity entity_id,
                                    const PriorityComponent &priority_comp,
                                    const SpriteComponent &sprite_comp,
                                    const TransformComponent &transform_comp) {
        uint32_t id = static_cast<uint32_t>(entity_id);
        auto &frame = sprite_comp.frame;
        auto sprite = asset->Get<Sprite>(frame.id);
        if (sprite) {
            datas.push_back({sprite->GetTexture(), frame.uvs,
                             transform_comp.GetWorldPosition(),
                             transform_comp.GetWorldRotation(), frame.size, id,
                             priority_comp.priority});
        }
    });
    auto anim_view = scene->view<PriorityComponent, SpriteAnimationComponent,
                                 TransformComponent>();
    anim_view.each([this, &datas](entt::entity entity_id,
                                  const PriorityComponent &priority_comp,
                                  const SpriteAnimationComponent &anim_comp,
                                  const TransformComponent &transform_comp) {
        uint32_t id = static_cast<uint32_t>(entity_id);
        auto &frames = anim_comp.animations.at(anim_comp.index);
        if (frames.GetFrameSize()) {
            auto &frame = frames.GetFrame();
            auto sprite = asset->Get<Sprite>(frame.id);
            if (sprite) {
                datas.push_back({sprite->GetTexture(), frame.uvs,
                                 transform_comp.GetWorldPosition(),
                                 transform_comp.GetWorldRotation(), frame.size,
                                 id, priority_comp.priority});
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
        } else if (l_z > r_z) {
            return false;
        } else {
            return l_p < r_p;
        }
    });
    for (const auto &data : datas) {
        renderer->DrawTexture(*data.texture, data.uvs, data.pos, data.rot,
                              data.size, glm::vec4(1.0f), data.entity_id);
    }

    auto textView = scene->view<TransformComponent, TextComponent>();

    textView.each([this](entt::entity entity_id,
                         const TransformComponent &transformComp,
                         const TextComponent &textComp) {
        renderer->SetTextOrigin(0, 0);
        auto font = asset->Get<Font>(textComp.id);
        if (font) {
            renderer->DrawText(*font, textComp.text,
                               transformComp.GetWorldTransform().GetMatrix(),
                               textComp.color,
                               static_cast<uint32_t>(entity_id));
        }
    });
    renderer->End();
    device->SetDepthMask(true);
}

}  // namespace SD
