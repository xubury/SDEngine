#ifndef SD_RESOURCE_REGISTRY_HPP
#define SD_RESOURCE_REGISTRY_HPP

#include <filesystem>
#include "Loader/TextureLoader.hpp"
#include "Loader/ModelLoader.hpp"
#include "Resource/ResourceCache.hpp"

namespace SD {

class ResourceRegistry {
   public:
    using TextureHandle = ResourceHandle<Texture>;
    using TextureCache = ResourceCache<Texture>;

    using ModelHandle = ResourceHandle<Model>;
    using ModelCache = ResourceCache<Model>;

    ResourceRegistry() = default;

    void SetDirectory(std::filesystem::path &&directory);

    template <typename... Args>
    TextureHandle LoadTexture(const ResourceId rid, Args &&...args)
    {
        return m_texture_cache.Load<TextureLoader>(rid,
                                                   std::forward<Args>(args)...);
    }

    template <typename... Args>
    ModelHandle LoadModel(const ResourceId rid, Args &&...args)
    {
        return m_model_cache.Load<ModelLoader>(rid, std::forward<Args>(args)...,
                                               m_texture_cache);
    }

    TextureCache &GetTextureCache() { return m_texture_cache; }
    const TextureCache &GetTextureCache() const { return m_texture_cache; }

    ModelCache &GetModelCache() { return m_model_cache; }
    const ModelCache &GetModelCache() const { return m_model_cache; }

   private:
    std::filesystem::path m_directory;
    TextureCache m_texture_cache;
    ModelCache m_model_cache;
};

}  // namespace SD
#endif
