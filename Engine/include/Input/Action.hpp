#ifndef SD_ACTION_HPP
#define SD_ACTION_HPP

#include <SDL.h>
#include "Utility/Base.hpp"

namespace SD {

class SD_API Action {
   public:
    enum Type {
        REAL_TIME = 1,
        PRESSED = 1 << 1,
        RELEASED = 1 << 2,
        DOWN = 1 << 3
    };

    Action(const SDL_EventType &event);

    Action(SDL_Keycode key, int type = Type::REAL_TIME | Type::PRESSED);

    Action(uint8_t button, int type = Type::REAL_TIME | Type::PRESSED);

    bool test() const;

    bool operator==(const SDL_Event &event) const;

    bool operator==(const Action &other) const;

   private:
    template <typename>
    friend class ActionTarget;

    SDL_Event m_event;
    int m_type;
};

}  // namespace SD
#endif /* SD_ACTION_HPP */
