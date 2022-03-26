#ifndef SD_MATH_HPP
#define SD_MATH_HPP

#include "Utility/Base.hpp"
#include "Utility/Export.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace SD {

using Vector2f = glm::vec2;
using Vector2i = glm::ivec2;

using Vector3f = glm::vec3;
using Vector3i = glm::ivec3;

using Vector4f = glm::vec4;
using Vector4i = glm::ivec4;

using Matrix3f = glm::mat3;
using Matrix4f = glm::mat4;

using Quaternion = glm::quat;

namespace Math {

inline constexpr float PI = 3.14159265358979323846;

struct SD_UTILITY_API Ray {
    Ray() = default;
    Ray(const Vector3f &origin, const Vector3f &direction)
        : origin(origin), direction(direction)
    {
    }
    Vector3f origin;
    Vector3f direction;
};

struct SD_UTILITY_API Plane {
    Plane(const Vector3f &normal, const Vector3f &point)
        : normal(normal), point(point)
    {
    }
    Vector3f normal;
    Vector3f point;
};

template <typename T>
inline T Lerp(T a, T b, float f)
{
    return a + f * (b - a);
}

template <glm::length_t L, typename T, glm::qualifier Q>
inline void BaryCentric(const glm::vec<L, T, Q> &a, const glm::vec<L, T, Q> &b,
                        const glm::vec<L, T, Q> &c,
                        const glm::vec<L, T, Q> &pos, float &u, float &v,
                        float &w)
{
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

inline bool IntersectRayPlane(const Ray &ray, const Plane &plane,
                              Vector3f &world)
{
    float numer = glm::dot(plane.normal, ray.origin) -
                  glm::dot(plane.normal, plane.point);
    float denom = glm::dot(plane.normal, ray.direction);

    // orthogonal, can't intercect
    if (std::fabs(denom) < std::numeric_limits<float>::epsilon()) {
        return false;
    }

    world = -numer / denom * ray.direction + ray.origin;
    return true;
}

bool SD_UTILITY_API Decompose(const Matrix4f &transform, Vector3f &translation,
                              Quaternion &rotation, Vector3f &scale);

}  // namespace Math

struct SD_UTILITY_API Rect {
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

}  // namespace SD

#endif /* SD_MATH_HPP */
