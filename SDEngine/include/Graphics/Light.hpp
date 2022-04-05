#ifndef SD_LIGHT_HPP
#define SD_LIGHT_HPP

#include "Graphics/Export.hpp"
#include "Utility/Base.hpp"
#include "Utility/Serialize.hpp"
#include "Utility/Math.hpp"

namespace SD {

struct SD_GRAPHICS_API DirectionalLight {
    Vector3f ambient;
    Vector3f diffuse;
    Vector3f specular;
    SERIALIZE(ambient, diffuse, specular)
};

struct SD_GRAPHICS_API PointLight {
    Vector3f ambient;
    Vector3f diffuse;
    Vector3f specular;
    float constant{1.0};
    float linear{0.1};
    float quadratic{0.01};
    SERIALIZE(ambient, diffuse, specular, constant, linear, quadratic)
};

}  // namespace SD

#endif /* SD_LIGHT_HPP */
