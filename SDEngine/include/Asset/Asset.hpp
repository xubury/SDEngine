#ifndef SD_ASSET_HPP
#define SD_ASSET_HPP

#include "Utility/Base.hpp"

#include "Asset/Export.hpp"
#include "Utility/String.hpp"
#include "Utility/Serialize.hpp"
#include "Utility/ResourceId.hpp"

namespace SD {

using TypeId = size_t;
template <typename T>
inline TypeId GetTypeId() {
    return typeid(T).hash_code();
}

class SD_ASSET_API Asset {
   public:
    Asset() : m_is_initialized(false){};
    virtual ~Asset() = default;

    virtual void Init() { m_is_initialized = true; };

    template <typename T>
    static void SaveArchiveToFile(const T *obj) {
        if (std::filesystem::exists(obj->m_path)) {
            std::filesystem::remove(obj->m_path);
        }
        std::ofstream os(obj->m_path, std::ios::binary);
        os << ASSET_IDENTIFIER;
        cereal::PortableBinaryOutputArchive oarchive(os);
        oarchive(obj->m_tid, obj->m_path, obj->m_rid, *obj);
    }

    template <typename T>
    static void LoadArchiveFromFile(const std::string &path, T *obj) {
        std::ifstream is(path, std::ios::binary);
        std::string id(ASSET_IDENTIFIER.size(), ' ');
        is.read(id.data(), id.size());
        if (id != ASSET_IDENTIFIER) {
            throw Exception("Invalid asset file!");
        }
        cereal::PortableBinaryInputArchive iarchive(is);
        iarchive(obj->m_tid, obj->m_path, obj->m_rid, *obj);
    }

    template <typename T, typename... ARGS>
    static Asset *Create(ARGS &&...args) {
        return dynamic_cast<Asset *>(new T(std::forward(args)...));
    }

    template <typename T>
    static void Destroy(Asset *ptr) {
        T *type_ptr = dynamic_cast<T *>(ptr);
        delete type_ptr;
    }

    ResourceId GetId() const { return m_rid; }

    const std::string &GetPath() const { return m_path; }

    bool IsInitialized() const { return m_is_initialized; }

    std::string GetAbsolutePath(const std::string &path) {
        return (std::filesystem::path(m_path).parent_path() / path)
            .generic_string();
    }

   protected:
    friend class AssetStorage;

    bool m_is_initialized;
    TypeId m_tid;
    ResourceId m_rid;
    std::string m_path;

   private:
    const static std::string ASSET_IDENTIFIER;
};

using AssetCreateFunc = std::function<Asset *()>;
using AssetDestroyFunc = std::function<void(Asset *ptr)>;

};  // namespace SD

#endif
