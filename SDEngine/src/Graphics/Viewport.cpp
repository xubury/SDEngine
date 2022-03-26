#include "Graphics/Viewport.hpp"

namespace SD {

Viewport::Viewport(int left, int top, int width, int height)
    : m_left(left), m_top(top), m_width(width), m_height(height)
{
}

int Viewport::GetLeft() const { return m_left; }

int Viewport::GetTop() const { return m_top; }

int Viewport::GetWidth() const { return m_width; }

int Viewport::GetHeight() const { return m_height; }

void Viewport::SetSize(int width, int height)
{
    m_width = width;
    m_height = height;
}

void Viewport::SetPos(int x, int y)
{
    m_left = x;
    m_top = y;
}

Vector2f Viewport::MapScreenToClip(const Vector2i &screen) const
{
    Vector2f clip(0);
    clip.x = 2.f * (screen.x - GetLeft()) / GetWidth() - 1.f;
    clip.y = 1.f - 2.f * (screen.y - GetTop()) / GetHeight();
    return clip;
}

Vector2i Viewport::MapClipToScreen(const Vector2f &clip) const
{
    Vector2f screen(0);
    screen.x = (clip.x + 1.f) * GetWidth() / 2.f + GetLeft();
    screen.y = (1.f - clip.y) * GetHeight() / 2.f + GetTop();
    return screen;
}

}  // namespace SD
