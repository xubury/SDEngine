#ifndef SD_LIGHT_HPP
#define SD_LIGHT_HPP

#include "Utils/Export.hpp"
#include <glm/glm.hpp>

namespace sd {

struct SD_API Light {
    glm::vec3 ambient = glm::vec3(1.0f);
    glm::vec3 diffuse = glm::vec3(1.0f);
    glm::vec3 specular = glm::vec3(1.0f);
    bool isDirectional = false;

    float cutOff = 25.f;
    float outerCutOff = 35.f;
    float constant = 1.0f;
    float linear = 0.09;
    float quadratic = 0.032f;
    template <typename Archive>
    void serialize(Archive& archive) {
        archive(ambient, diffuse, specular, isDirectional, cutOff, outerCutOff,
                constant, linear, quadratic);
    }
};

}  // namespace sd

#endif /* SD_LIGHT_HPP */
