#include "Utility/Math.hpp"

namespace SD {

Rect::Rect(float x, float y, float width, float height)
    : x(x), y(y), width(width), height(height) {}

bool Rect::Contains(const Rect &other) const {
    return x <= other.x && y <= other.y && x + width >= other.x + other.width &&
           y + height >= other.y + other.height;
}

bool Rect::Intersects(const Rect &other) const {
    return x <= other.x + other.width && x + width >= other.x &&
           y <= other.y + other.height && y + height >= other.y;
}

float Rect::GetLeft() const { return x - width * 0.5; }
float Rect::GetTop() const { return y + height * 0.5; }
float Rect::GetRight() const { return x + width * 0.5; }
float Rect::GetBottom() const { return y - height * 0.5; }

}  // namespace SD
