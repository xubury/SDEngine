#ifndef SD_LIGHT_HPP
#define SD_LIGHT_HPP

#include "Common/Export.hpp"
#include <glm/glm.hpp>

namespace sd {

struct SD_API Light {
    glm::vec3 ambient = glm::vec3(1.0f);
    glm::vec3 diffuse = glm::vec3(1.0f);
    glm::vec3 specular = glm::vec3(1.0f);
};

}  // namespace sd

#endif /* SD_LIGHT_HPP */
