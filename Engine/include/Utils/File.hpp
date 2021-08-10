#ifndef SD_FILE_HPP
#define SD_FILE_HPP

#include <string>
#include "Common/Export.hpp"

namespace sd {

void SD_API readFile(const std::string &filePath, std::string &content);

void SD_API writeFile(const std::string &filePath, const std::string &content);

}  // namespace sd

#endif /* SD_FILE_HPP */
