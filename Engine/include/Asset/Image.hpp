#ifndef SD_IMAGE_HPP
#define SD_IMAGE_HPP

#include "Utility/Base.hpp"
#include "Asset/Export.hpp"
#include <string>

struct SDL_Surface;

namespace SD {

class SD_ASSET_API Image {
   public:
    Image(SDL_Surface *surface);
    ~Image();

    Image(const Image &) = delete;
    Image &operator=(const Image &) = delete;

    bool Valid() const;

    const void *Data() const;
    void *Data();

    uint32_t Width() const;
    uint32_t Height() const;

    bool HasAlpha() const;

   private:
    SDL_Surface *m_surface;
};

}  // namespace SD

#endif  // SD_IMAGE_HPP
