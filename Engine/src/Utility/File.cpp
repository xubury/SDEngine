#include "Utility/File.hpp"
#include "Utility/Log.hpp"
#include <fstream>
#include <sstream>

namespace SD {

namespace File {

void Read(const std::string &filePath, std::string &content) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(filePath);
        std::stringstream fileStream;
        fileStream << file.rdbuf();
        file.close();
        content = fileStream.str();
    } catch (std::ifstream::failure &e) {
        throw FileException(filePath, std::strerror(errno));
    }
}

}  // namespace File

}  // namespace SD
