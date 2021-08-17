#ifndef SD_EDITOR_HPP
#define SD_EDITOR_HPP

#include "Core/Application.hpp"

class SDEditor : public sd::Application {
   public:
    void init() override;
    void destroy() override;
};

#endif /* SD_EDITOR_HPP */
