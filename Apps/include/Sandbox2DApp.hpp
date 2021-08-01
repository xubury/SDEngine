#ifndef SIMPLE2DAPP_H
#define SIMPLE2DAPP_H

#include "Core/Application.hpp"

class Sandbox2DApp : public sd::Application {
   public:
    void init() override;
    void destroy() override;
};

#endif /* SIMPLE2DAPP_H */
