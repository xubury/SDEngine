#ifndef SD_RESOURCE_HPP
#define SD_RESOURCE_HPP

#include "Resource/Export.hpp"
#include "Resource/TextureLoader.hpp"
#include "Resource/ModelLoader.hpp"
#include "Resource/ImageLoader.hpp"
#include "Resource/ShaderLoader.hpp"
#include "Resource/ResourceCache.hpp"

namespace SD {

template <typename LoaderType>
struct Loader : LoaderType {
    template <typename... Args>
    auto operator()(Args &&...args)
    {
        return LoaderType::Load(std::forward<Args>(args)...);
    }
};

using ImageHandle = ResourceHandle<ByteImage>;
using ImageCache = ResourceCache<ByteImage, Loader<ImageLoader>>;

using TextureHandle = ResourceHandle<Texture>;
using TextureCache = ResourceCache<Texture, Loader<TextureLoader>>;

using ModelHandle = ResourceHandle<Model>;
using ModelCache = ResourceCache<Model, Loader<ModelLoader>>;

using ShaderCache = ResourceCache<Shader, Loader<ShaderLoader>>;
using ShaderHandle = ResourceHandle<Shader>;

}  // namespace SD

#endif
