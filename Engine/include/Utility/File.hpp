#ifndef SD_FILE_HPP
#define SD_FILE_HPP

#include <string>
#include "Utility/Export.hpp"

namespace sd {

namespace File {

void SD_API read(const std::string &filePath, std::string &content);

void SD_API write(const std::string &filePath, const std::string &content);

}  // namespace File

}  // namespace sd

#endif /* SD_FILE_HPP */
