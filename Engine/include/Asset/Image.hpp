#ifndef SD_IMAGE_HPP
#define SD_IMAGE_HPP

#include <string>

struct SDL_Surface;

namespace SD {

class Image {
   public:
    Image(const std::string &name);
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
