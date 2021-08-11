#ifndef SANDBOX_2D_APP_HPP
#define SANDBOX_2D_APP_HPP

#include "Core/Application.hpp"

class Sandbox2DApp : public sd::Application {
   public:
    void init() override;
    void destroy() override;
};

#endif /* SANDBOX_2D_APP_HPP */
