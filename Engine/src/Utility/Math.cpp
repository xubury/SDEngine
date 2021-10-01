#include "Utility/Math.hpp"

namespace sd {

Rect::Rect(float x, float y, float width, float height)
    : x(x), y(y), width(width), height(height) {}

bool Rect::contains(const Rect &other) const {
    return x <= other.x && y <= other.y && x + width >= other.x + other.width &&
           y + height >= other.y + other.height;
}

bool Rect::intersects(const Rect &other) const {
    return x <= other.x + other.width && x + width >= other.x &&
           y <= other.y + other.height && y + height >= other.y;
}

float Rect::getLeft() const { return x - width * 0.5; }
float Rect::getTop() const { return y + height * 0.5; }
float Rect::getRight() const { return x + width * 0.5; }
float Rect::getBottom() const { return y - height * 0.5; }

}  // namespace sd
