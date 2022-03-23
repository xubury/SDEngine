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

    void SetSize(int width, int height);
    void SetPos(int x, int y);
    glm::vec2 MapScreenToClip(const glm::ivec2 &screen) const;
    glm::ivec2 MapClipToScreen(const glm::vec2 &clip) const;

   private:
    int m_left;
    int m_top;
    int m_width;
    int m_height;
};

}  // namespace SD

#endif  // !SD_VIEWPORT_HPP
