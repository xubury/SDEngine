#ifndef SD_RESOURCE_REGISTRY_HPP
#define SD_RESOURCE_REGISTRY_HPP

#include <filesystem>
#include "Loader/TextureLoader.hpp"
#include "Loader/ModelLoader.hpp"
#include "Loader/ImageLoader.hpp"
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

class ResourceRegistry {
   public:
    using ImageHandle = ResourceHandle<ByteImage>;
    using ImageCache = ResourceCache<ByteImage, Loader<ImageLoader>>;

    using TextureHandle = ResourceHandle<Texture>;
    using TextureCache = ResourceCache<Texture, Loader<TextureLoader>>;

    using ModelHandle = ResourceHandle<Model>;
    using ModelCache = ResourceCache<Model, Loader<ModelLoader>>;

    ResourceRegistry() = default;

    void SetDirectory(std::filesystem::path &&directory);

    ImageHandle LoadImage(const ResourceId rid, const std::string &path)
    {
        return m_image_cache.Load(rid, path);
    }

    TextureHandle LoadTexture(const ResourceId rid, const std::string &path,
                              TextureWrap wrap, TextureMinFilter min_filter,
                              TextureMagFilter mag_filter, MipmapMode mode)
    {
        return m_texture_cache.Load(rid, path, wrap, min_filter, mag_filter,
                                    mode, *this);
    }

    ModelHandle LoadModel(const ResourceId rid, const std::string &path)
    {
        return m_model_cache.Load(rid, path, *this);
    }

    TextureCache &GetTextureCache() { return m_texture_cache; }
    const TextureCache &GetTextureCache() const { return m_texture_cache; }

    ModelCache &GetModelCache() { return m_model_cache; }
    const ModelCache &GetModelCache() const { return m_model_cache; }

   private:
    std::filesystem::path m_directory;
    ImageCache m_image_cache;
    TextureCache m_texture_cache;
    ModelCache m_model_cache;
};

}  // namespace SD

#endif
