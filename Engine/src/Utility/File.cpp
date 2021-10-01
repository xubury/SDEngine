#include "Utility/File.hpp"
#include "Utility/Log.hpp"
#include <fstream>
#include <sstream>

namespace sd {

void readFile(const std::string &filePath, std::string &content) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(filePath);
        std::stringstream fileStream;
        fileStream << file.rdbuf();
        file.close();
        content = fileStream.str();
    } catch (std::ifstream::failure &e) {
        SD_CORE_ERROR("File from {} are not successfully read! Error: {}",
                      filePath, e.what());
    }
}

}  // namespace sd
