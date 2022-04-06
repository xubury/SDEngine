#ifndef SD_RENDERER_2D_HPP
#define SD_RENDERER_2D_HPP

#include "Renderer/Renderer.hpp"

namespace SD {

class SD_RENDERER_API Renderer2D : protected Renderer {
   public:
    static void Init();
    static void Begin();
    static void End();

    static void Reset();
    static std::string GetDebugInfo();

    static void SetTextOrigin(int x, int y);
    static Vector2i GetTextCursor();

    static void DrawLine(const Vector3f &start, const Vector3f &end,
                         const Vector4f &color, uint32_t entity_id = -1);

    static void DrawQuad(const Vector3f &pos, const Quaternion &rot,
                         const Vector2f &scale, const Vector4f &color,
                         uint32_t entity_id = -1);
    static void DrawQuad(const Matrix4f &transform, const Vector4f &color,
                         uint32_t entity_id = -1);

    static void DrawTexture(const Texture &texture,
                            const std::array<Vector2f, 2> &uv,
                            const Vector3f &pos, const Quaternion &rot,
                            const Vector2f &scale,
                            const Vector4f &color = Vector4f(1.0f),
                            uint32_t entity_id = -1);
    static void DrawTexture(const Texture &texture,
                            const std::array<Vector2f, 2> &uv,
                            const Matrix4f &transform,
                            const Vector4f &color = Vector4f(1.0f),
                            uint32_t entity_id = -1);

    static void DrawTexture(const Texture &texture, const Vector3f &pos,
                            const Quaternion &rot, const Vector2f &scale,
                            const Vector4f &color = Vector4f(1.0f),
                            uint32_t entity_id = -1);
    static void DrawTexture(const Texture &texture, const Matrix4f &transform,
                            const Vector4f &color = Vector4f(1.0f),
                            uint32_t entity_id = -1);

    static void DrawBillboard(const Texture &texture,
                              const std::array<Vector2f, 2> &uv,
                              const Vector3f &pos, const Vector2f &scale,
                              const Vector4f &color = Vector4f(1.0f),
                              uint32_t entity_id = -1);
    static void DrawBillboard(const Texture &texture, const Vector3f &pos,
                              const Vector2f &scale,
                              const Vector4f &color = Vector4f(1.0f),
                              uint32_t entity_id = -1);

    static void DrawText(const Font &font, const std::string &text,
                         const Matrix4f &transform,
                         const Vector4f &color = Vector4f(1.0f),
                         uint32_t entity_id = -1);

    static void DrawCircle(const Vector3f &pos, const Vector2f &scale,
                           const Vector4f &color, float thickness, float fade,
                           uint32_t entity_id = -1);
    static void DrawCircle(const Matrix4f &transform, const Vector4f &color,
                           float thickness, float fade,
                           uint32_t entity_id = -1);

   private:
    static void StartBatch();
    static void Flush();

    static void StartLineBatch();
    static void StartQuadBatch();
    static void StartCircleBatch();

    static void FlushLines();
    static void FlushQuads();
    static void FlushCircles();

    static void NextLineBatch();
    static void NextQuadBatch();
    static void NextCircleBatch();
};

}  // namespace SD
#endif
