#ifndef SD_ASSET_HPP
#define SD_ASSET_HPP

#include "Utility/Base.hpp"

#include "Asset/Export.hpp"

namespace SD {

template <typename ASSET>
inline size_t GetAssetType() {
    return typeid(ASSET).hash_code();
}

class SD_ASSET_API Asset {
   public:
    Asset();
    Asset(size_t loaderType, const std::string &path);

    size_t GetLoaderType() const { return m_loader_type; }
    void SetLoaderType(size_t type) { m_loader_type = type; }

    Ref<void> GetResource() { return m_resource; }
    const Ref<void> GetResource() const { return m_resource; }

    void SetResource(const Ref<void> &resource) { m_resource = resource; }

    const std::string &GetPath() const { return m_path; }

    template <typename Archive>
    void serialize(Archive &archive) {
        archive(m_loader_type, m_path);
    }

   private:
    Ref<void> m_resource;
    size_t m_loader_type;
    std::string m_path;
};

};  // namespace SD

#endif
