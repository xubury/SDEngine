#include "Utils/Math.hpp"

namespace sd {

AABB::AABB() : min(FLT_MAX), max(-FLT_MAX) {}

void AABB::updateBounds(const glm::vec3 &pos) {
    min = glm::min(pos, min);
    max = glm::max(pos, max);
}

}  // namespace sd
