#ifndef SD_MATH_HPP
#define SD_MATH_HPP

#include <glm/glm.hpp>

namespace SD {

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};

struct Rect {
    float x;
    float y;
    float width;
    float height;
    bool Contains(const Rect &other) const;
    bool Intersects(const Rect &other) const;
    float GetLeft() const;
    float GetTop() const;
    float GetRight() const;
    float GetBottom() const;

    Rect(float x = 0, float y = 0, float width = 0, float height = 0);
};

template <glm::length_t L, typename T, glm::qualifier Q>
inline void BaryCentric(const glm::vec<L, T, Q> &a, const glm::vec<L, T, Q> &b,
                        const glm::vec<L, T, Q> &c,
                        const glm::vec<L, T, Q> &pos, float &u, float &v,
                        float &w) {
    glm::vec<L, T, Q> v0 = b - a;
    glm::vec<L, T, Q> v1 = c - a;
    glm::vec<L, T, Q> v2 = pos - a;
    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    float invDenom = 1.f / (d00 * d11 - d01 * d01);
    v = (d11 * d20 - d01 * d21) * invDenom;
    w = (d00 * d21 - d01 * d20) * invDenom;
    u = 1.0f - v - w;
}

}  // namespace SD

#endif /* SD_MATH_HPP */
