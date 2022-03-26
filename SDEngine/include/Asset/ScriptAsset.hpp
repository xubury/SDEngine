#ifndef SD_SCRIPT_ASSET_HPP
#define SD_SCRIPT_ASSET_HPP

#include "Asset/Asset.hpp"
#include "Utility/Serialize.hpp"

namespace SD {

class ScriptAsset : public Asset {
   public:
    ScriptAsset() {}

    void Serialize(cereal::PortableBinaryOutputArchive &archive) override
    {
        archive(*this);
    }

    void Deserialize(cereal::PortableBinaryInputArchive &archive) override
    {
        archive(*this);
    }

    SERIALIZE(m_code)
   private:
    std::string m_code;
};

}  // namespace SD
#endif
