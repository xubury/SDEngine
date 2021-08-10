#include "Utils/File.hpp"
#include "Common/Log.hpp"
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
        SD_CORE_ERROR("File not successfully read! Path: {}", filePath);
    }
}

}  // namespace sd
