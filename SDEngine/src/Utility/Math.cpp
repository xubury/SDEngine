#include "Utility/Math.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace SD {

namespace Math {

bool Decompose(const Matrix4f &transform, Vector3f &translation,
               Quaternion &rotation, Vector3f &scale)
{
    // From glm::decompose in matrix_decompose.inl

    using namespace glm;
    using T = float;

    mat4 LocalMatrix(transform);

    // Normalize the matrix.
    if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
        return false;

    // First, isolate perspective.  This is the messiest.
    if (epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>())) {
        // Clear the perspective partition
        LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] =
            static_cast<T>(0);
        LocalMatrix[3][3] = static_cast<T>(1);
    }

    // Next take care of translation (easy).
    translation = vec3(LocalMatrix[3]);
    LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

    vec3 Row[3];

    // Now get scale and shear.
    for (length_t i = 0; i < 3; ++i)
        for (length_t j = 0; j < 3; ++j) Row[i][j] = LocalMatrix[i][j];

    // Compute X scale factor and normalize first row.
    scale.x = length(Row[0]);
    Row[0] = detail::scale(Row[0], static_cast<T>(1));
    scale.y = length(Row[1]);
    Row[1] = detail::scale(Row[1], static_cast<T>(1));
    scale.z = length(Row[2]);
    Row[2] = detail::scale(Row[2], static_cast<T>(1));

    int i, j, k = 0;
    T root, trace = Row[0].x + Row[1].y + Row[2].z;
    if (trace > static_cast<T>(0)) {
        root = sqrt(trace + static_cast<T>(1.0));
        rotation.w = static_cast<T>(0.5) * root;
        root = static_cast<T>(0.5) / root;
        rotation.x = root * (Row[1].z - Row[2].y);
        rotation.y = root * (Row[2].x - Row[0].z);
        rotation.z = root * (Row[0].y - Row[1].x);
    }  // End if > 0
    else {
        static int Next[3] = {1, 2, 0};
        i = 0;
        if (Row[1].y > Row[0].x) i = 1;
        if (Row[2].z > Row[i][i]) i = 2;
        j = Next[i];
        k = Next[j];

        root = sqrt(Row[i][i] - Row[j][j] - Row[k][k] + static_cast<T>(1.0));

        rotation[i] = static_cast<T>(0.5) * root;
        root = static_cast<T>(0.5) / root;
        rotation[j] = root * (Row[i][j] + Row[j][i]);
        rotation[k] = root * (Row[i][k] + Row[k][i]);
        rotation.w = root * (Row[j][k] - Row[k][j]);
    }  // End if <= 0

    return true;
}
}  // namespace Math

Rect::Rect(float x, float y, float width, float height)
    : x(x), y(y), width(width), height(height)
{
}

bool Rect::Contains(const Rect &other) const
{
    return x <= other.x && y <= other.y && x + width >= other.x + other.width &&
           y + height >= other.y + other.height;
}

bool Rect::Intersects(const Rect &other) const
{
    return x <= other.x + other.width && x + width >= other.x &&
           y <= other.y + other.height && y + height >= other.y;
}

float Rect::GetLeft() const { return x - width * 0.5; }
float Rect::GetTop() const { return y + height * 0.5; }
float Rect::GetRight() const { return x + width * 0.5; }
float Rect::GetBottom() const { return y - height * 0.5; }

}  // namespace SD
