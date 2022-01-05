#ifndef SD_VIEWPORT_HPP
#define SD_VIEWPORT_HPP

#include "Utility/Base.hpp"
#include "Graphics/Export.hpp"

namespace SD {

class SD_GRAPHICS_API Viewport {
   public:
    Viewport() = default;
    Viewport(int left, int top, int width, int height);

    int GetLeft() const;
    int GetTop() const;

    int GetWidth() const;
    int GetHeight() const;

    void SetSize(int left, int top, int width, int height);
    glm::vec2 MapScreenToClip(const glm::ivec2 &screen) const;
    glm::ivec2 MapClipToScreen(const glm::vec2 &clip) const;

    void SetHover(bool hover) { m_is_hover = hover; }

    bool IsHover() const { return m_is_hover; }

    void SetFocus(bool focus) { m_is_focus = focus; }

    bool IsFocus() const { return m_is_focus; }

   private:
    int m_left;
    int m_top;
    int m_width;
    int m_height;

    bool m_is_hover;
    bool m_is_focus;
};
}  // namespace SD

#endif  // !SD_VIEWPORT_HPP
