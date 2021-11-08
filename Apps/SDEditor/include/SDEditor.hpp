#ifndef SD_EDITOR_HPP
#define SD_EDITOR_HPP

#include "Core/Application.hpp"

namespace sd {

class SDEditor : public Application {
   public:
    void init() override;
    void destroy() override;
};

}  // namespace sd

#endif /* SD_EDITOR_HPP */
