#include "Utility/IniParser.hpp"
#include <fstream>

namespace SD {

IniParser::IniParser(const std::string& filename) : m_filenname(filename) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(filename);
        ParseStream(file);
        file.close();
    } catch (std::ifstream::failure& e) {
        SD_CORE_ERROR("File from {} are not successfully read! Error: {}",
                      filename, e.what());
    }
}

IniParser::~IniParser() {}

}  // namespace SD
