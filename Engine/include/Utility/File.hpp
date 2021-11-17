#ifndef SD_FILE_HPP
#define SD_FILE_HPP

#include <string>
#include "Utility/Export.hpp"

namespace SD {

namespace File {

void SD_API Read(const std::string &filePath, std::string &content);

void SD_API Write(const std::string &filePath, const std::string &content);

}  // namespace File

}  // namespace SD

#endif /* SD_FILE_HPP */
