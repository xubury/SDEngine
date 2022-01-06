#ifndef SD_ASSET_MANAGER_HPP
#define SD_ASSET_MANAGER_HPP

#include "Utility/Base.hpp"
#include "Utility/ResourceId.hpp"
#include "Utility/Serialize.hpp"

#include "Asset/Export.hpp"
#include "Asset/Asset.hpp"

#include <string>
#include <unordered_map>
#include <typeinfo>
#include <filesystem>
#include <shared_mutex>

namespace SD {
class AssetManager;

class SD_ASSET_API AssetLoader {
   public:
    AssetLoader(AssetManager &manager) : m_manager(manager) {}
    AssetLoader(const AssetLoader &) = delete;
    AssetLoader &operator=(const AssetLoader &) = delete;
    virtual ~AssetLoader() = default;

    virtual Ref<void> LoadAsset(const std::string &path) = 0;

    AssetManager &Manager() { return m_manager; }

   private:
    AssetManager &m_manager;
};

class SD_ASSET_API AssetManager {
   private:
    void Clear();

    Ref<void> Cache(size_t type, const std::string &rel_path);

   public:
    AssetManager();
    ~AssetManager();

    void Load(const std::filesystem::path &path);
    void Save();

    void SetDirectory(const std::filesystem::path &path);
    std::filesystem::path GetRootPath() const;

    std::filesystem::path GetRelativePath(
        const std::filesystem::path &path) const;
    std::filesystem::path GetAbsolutePath(
        const std::filesystem::path &path) const;

    bool HasId(const std::string &path) const;

    bool Valid() const { return std::filesystem::exists(m_directory); }

    template <typename ASSET, typename LOADER, typename... ARGS>
    void SetLoader(ARGS &&...args) {
        size_t type = GetAssetType<ASSET>();
        m_loaders[type] = new LOADER(*this, std::forward(args)...);
    }

    template <typename ASSET>
    ResourceId LoadAsset(const std::filesystem::path &path) {
        SD_CORE_ASSERT(Valid(), "AssetManager's root path is invalid!");
        std::filesystem::path full_path = GetAbsolutePath(path);
        std::string rel_path = GetRelativePath(full_path).generic_string();
        std::string string_id = GetStringId<ASSET>(rel_path);
        // check if the relative path has id
        if (HasId(string_id)) {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            return m_id_map.at(string_id);
        } else {
            // generate a random id
            ResourceId id;
            size_t type = GetAssetType<ASSET>();
            Ref<void> resource = Cache(type, rel_path);
            {
                std::lock_guard<std::shared_mutex> lock(m_mutex);
                m_id_map.emplace(string_id, id);
                m_resources.emplace(id, Asset(type, rel_path));
                m_resources.at(id).SetResource(resource);
            }
            return id;
        }
    }

    template <typename ASSET>
    Ref<ASSET> Get(ResourceId id) {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        if (m_resources.count(id) == 0) {
            return nullptr;
        }
        Ref<void> res = m_resources.at(id).GetResource();
        lock.unlock();
        if (res == nullptr) {
            res = Cache(GetAssetType<ASSET>(), GetAssetPath(id));
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            m_resources.at(id).SetResource(res);
        }
        return std::static_pointer_cast<ASSET>(res);
    }

    // Return relative path
    std::string GetAssetPath(ResourceId id) {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        if (m_resources.count(id) == 0) {
            return "";
        }
        return m_resources.at(id).GetPath();
    }

    template <typename ASSET>
    Ref<ASSET> LoadAndGet(const std::filesystem::path &path) {
        auto id = LoadAsset<ASSET>(path);
        return Get<ASSET>(id);
    }

    template <typename ASSET>
    AssetLoader *GetLoader() {
        size_t type = GetAssetType<ASSET>();
        SD_CORE_ASSERT(m_loaders.count(type), "Loader not set up correctly!");
        return m_loaders.at(type);
    }

    void Validate();

   private:
    std::unordered_map<std::string, ResourceId> m_id_map;
    std::unordered_map<ResourceId, Asset> m_resources;
    std::unordered_map<size_t, AssetLoader *> m_loaders;
    std::filesystem::path m_directory;
    mutable std::shared_mutex m_mutex;

    template <typename ASSET>
    std::string GetStringId(const std::string &str) {
        std::string ret = typeid(ASSET).name();
        ret = ret + "@" + str;
        return ret;
    }

    std::string GetPathFromId(const std::string &str) {
        size_t sep_pos = str.find_first_of('@');
        if (sep_pos == std::string::npos) {
            SD_CORE_WARN("Invalid id map");
            return "";
        }
        std::string path = str.substr(sep_pos + 1);
        return path;
    }
};

}  // namespace SD

#endif
