#ifndef SD_ENTRY_POINT_HPP
#define SD_ENTRY_POINT_HPP

#include "Core/Application.hpp"

namespace sd {

#define IMPLEMENT_APP(x) \
    ::sd::Application * ::sd::createApp() { return new x; }

extern SD_API Application *createApp();

};  // namespace sd

#endif /* SD_ENTRY_POINT_HPP */
