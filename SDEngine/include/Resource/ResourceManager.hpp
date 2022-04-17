#ifndef SD_RESOURCE_MANAGER_HPP
#define SD_RESOURCE_MANAGER_HPP

#include <filesystem>
#include "Resource/Export.hpp"
#include "Resource/TextureLoader.hpp"
#include "Resource/ModelLoader.hpp"
#include "Resource/ImageLoader.hpp"
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

class SD_RESOURCE_API ResourceManager {
   public:
    static ResourceManager &Get();

    void Init(std::filesystem::path directory);
    const std::filesystem::path &GetDirectory() { return m_directory; }

    ImageHandle LoadImage(const std::string &path)
    {
        return m_image_cache.Load(ResourceId(path), m_directory / path);
    }

    TextureHandle LoadTexture(const ResourceId rid, const std::string &path,
                              const TextureParameter &param)
    {
        return m_texture_cache.Load(rid, m_directory / path, param);
    }

    ModelHandle LoadModel(const ResourceId rid, const std::string &path)
    {
        return m_model_cache.Load(rid, m_directory / path);
    }

    TextureCache &GetTextureCache() { return m_texture_cache; }
    const TextureCache &GetTextureCache() const { return m_texture_cache; }

    ModelCache &GetModelCache() { return m_model_cache; }
    const ModelCache &GetModelCache() const { return m_model_cache; }

   private:
    ResourceManager() = default;
    ~ResourceManager() = default;

    std::filesystem::path m_directory;
    ImageCache m_image_cache;
    TextureCache m_texture_cache;
    ModelCache m_model_cache;
};

}  // namespace SD

#endif
