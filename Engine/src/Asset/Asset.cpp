#include "Asset/Asset.hpp"

namespace SD {

Asset::Asset() : m_resource(nullptr) {}

Asset::Asset(size_t loader_type, const std::string &path)
    : m_resource(nullptr), m_loader_type(loader_type), m_path(path) {}

}  // namespace SD
