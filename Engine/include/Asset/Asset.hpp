#ifndef SD_ASSET_HPP
#define SD_ASSET_HPP

#include "Utility/Export.hpp"
#include "Utility/ResourceId.hpp"
#include <string>

namespace sd {

class SD_API Asset {
   public:
    Asset(const std::string &name);

   private:
    ResourceId m_id;
    std::string m_name;
};

};  // namespace sd

#endif
