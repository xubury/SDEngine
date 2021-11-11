#ifndef SD_ENTRY_POINT_HPP
#define SD_ENTRY_POINT_HPP

#include "Core/Application.hpp"

namespace SD {

#define IMPLEMENT_APP(x) \
    ::SD::Application * ::SD::createApp() { return new x; }

extern SD_API Application *createApp();

};  // namespace SD

#endif /* SD_ENTRY_POINT_HPP */
