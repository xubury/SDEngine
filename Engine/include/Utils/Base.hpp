#ifndef SD_BASE_HPP
#define SD_BASE_HPP

#include <memory>
#include <cstdint>
#include "Utils/PlatformDetection.hpp"

namespace sd {

template <typename T>
using Scope = std::unique_ptr<T>;
template <typename T, typename... Args>
constexpr Scope<T> createScope(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using Ref = std::shared_ptr<T>;
template <typename T, typename... Args>
constexpr Ref<T> createRef(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

}  // namespace sd


#endif
