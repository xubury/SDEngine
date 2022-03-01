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
    Asset() : m_is_initialized(false){};
    virtual ~Asset() = default;

    virtual void Init() { m_is_initialized = true; };

    template <typename T>
    static void SaveArchiveToFile(const std::string &path, const T *obj) {
        if (std::filesystem::exists(path)) {
            std::filesystem::remove(path);
        }
        std::ofstream os(path, std::ios::binary);
        os << ASSET_IDENTIFIER;
        cereal::PortableBinaryOutputArchive oarchive(os);
        oarchive(*obj);
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
        iarchive(*obj);
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

    void SetStringId(const std::string &path) {
        m_path = path;
        m_rid = std::hash<std::string>{}(path);
    }

    const static std::string ASSET_IDENTIFIER;

    bool IsInitialized() const { return m_is_initialized; }

    std::string GetAbsolutePath(const std::string &path) {
        return (std::filesystem::path(m_path).parent_path() / path)
            .generic_string();
    }

   protected:
    friend class AssetStorage;

    bool m_is_initialized;
    ResourceId m_rid;
    std::string m_path;
};

using AssetCreateFunc = std::function<Asset *()>;
using AssetDestroyFunc = std::function<void(Asset *ptr)>;

};  // namespace SD

#endif
