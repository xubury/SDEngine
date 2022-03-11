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
    Asset(){};
    virtual ~Asset() = default;

    virtual void Deserialize(cereal::PortableBinaryInputArchive &archive) = 0;
    virtual void Serialize(cereal::PortableBinaryOutputArchive &archive) = 0;

    template <typename T, typename... ARGS>
    static Asset *Create(ARGS &&...args) {
        return dynamic_cast<Asset *>(new T(std::forward(args)...));
    }

    static void Destroy(Asset *ptr) { delete ptr; }

    ResourceId GetId() const { return m_rid; }

    std::string GetName() const { return m_name; }

   protected:
    friend class AssetStorage;

    bool m_is_initialized;
    TypeId m_tid;
    ResourceId m_rid;
    std::string m_name;
};

using AssetCreateFunc = std::function<Asset *()>;
using AssetDestroyFunc = std::function<void(Asset *ptr)>;

};  // namespace SD

#endif
