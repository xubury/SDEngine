#ifndef SD_ASSET_HPP
#define SD_ASSET_HPP

#include "Utility/Base.hpp"

#include "Asset/Export.hpp"
#include "Utility/String.hpp"
#include "Utility/Serialize.hpp"
#include "Utility/ResourceId.hpp"

namespace SD {

using TypeId = size_t;

class SD_ASSET_API Asset {
   public:
    virtual ~Asset() = default;

    virtual void *LoadFromFile(const std::string &path) = 0;

    template <typename T, typename... ARGS>
    static Asset *Create(ARGS &&...args) {
        return dynamic_cast<Asset *>(new T(std::forward(args)...));
    }

    template <typename T>
    static void Destroy(Asset *ptr) {
        T *type_ptr = dynamic_cast<T *>(ptr);
        delete type_ptr;
    }

    ResourceId GetStringId() const { return m_rid; }

    const std::string &GetPath() const { return m_path; }

    void SetStringId(const std::string &path) {
        m_path = path;
        m_rid = std::hash<std::string>{}(path);
    }

    template <typename T>
    static void SaveArchiveToFile(const std::string &path, const T &obj) {
        if (std::filesystem::exists(path)) {
            std::filesystem::remove(path);
        }
        std::ofstream os(path, std::ios::binary);
        cereal::PortableBinaryOutputArchive oarchive(os);
        oarchive(obj);
    }

    template <typename T>
    static T LoadArchiveFromFile(const std::string &path) {
        T obj;
        std::ifstream is(path, std::ios::binary);
        cereal::PortableBinaryInputArchive iarchive(is);
        iarchive(obj);
        return obj;
    }

    // template <typename T, typename... ARGS>
    // static T *GetCreateAssetData(AssetStorage *storage,
    //                              const std::string &path) {
    //     ResourceId sid = std::hash<std::string>{}(path);
    //     T *data = nullptr;
    //     if (storage->Exists<T>(sid)) {
    //         data = storage->GetAsset<T>(sid);
    //     } else {
    //         data = new T();
    //         data->m_sid = sid;
    //         data->m_path = path;
    //         SaveArchiveToFile(path, *data);
    //         storage->Add(static_cast<void *>(data), GetTypeId<T>(), sid);
    //     }
    //     return data;
    // }

   protected:
    friend class AssetStorage;

    ResourceId m_rid;
    std::string m_path;

    Asset() = default;
};

using AssetCreateFunc = std::function<Asset *()>;
using AssetDestroyFunc = std::function<void(Asset *ptr)>;

};  // namespace SD

#endif
