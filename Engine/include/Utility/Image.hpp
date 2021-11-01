#ifndef SD_IMAGE_HPP
#define SD_IMAGE_HPP

#include <string>

struct SDL_Surface;

namespace sd {

class Image {
   public:
    Image(const std::string &name);
    ~Image();

    Image(const Image &) = delete;
    Image &operator=(const Image &) = delete;

    bool valid() const;

    const void *data() const;
    void *data();

    uint32_t width() const;
    uint32_t height() const;

    bool hasAlpha() const;

   private:
    SDL_Surface *m_surface;
};

}  // namespace sd

#endif  // SD_IMAGE_HPP
