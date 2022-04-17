#ifndef SD_LOCATOR_HPP
#define SD_LOCATOR_HPP

#include "Utility/Base.hpp"

namespace SD {

template <typename Service>
class Locator {
   public:
    Locator() = delete;
    ~Locator() = delete;

    static bool HasValue() { return static_cast<bool>(m_service); }

    static Service& Value()
    {
        SD_CORE_ASSERT(HasValue(), "Service not avaliable!");
        return *m_service;
    }

    template <typename... Args>
    static Service& Emplace(Args&&... args)
    {
        m_service = CreateRef<Service>(std::forward<Args>(args)...);
        return *m_service;
    }

    static void Reset() { m_service.reset(); }

   private:
    inline static Ref<Service> m_service{nullptr};
};

}  // namespace SD
#endif
