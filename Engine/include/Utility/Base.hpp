#ifndef SD_BASE_HPP
#define SD_BASE_HPP

#include <memory>
#include <cstdint>

namespace SD {

template <typename T>
using Scope = std::unique_ptr<T>;
template <typename T, typename... Args>
constexpr Scope<T> CreateScope(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using Ref = std::shared_ptr<T>;
template <typename T, typename... Args>
constexpr Ref<T> CreateRef(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

}  // namespace SD

#include "Utility/Export.hpp"
#include "Utility/Log.hpp"
#include "Utility/Assert.hpp"
#include "Utility/Exception.hpp"

#endif
