#ifndef SD_FILE_HPP
#define SD_FILE_HPP

#include "Utility/Base.hpp"
#include "Utility/Exception.hpp"
#include <string>

namespace SD {

namespace File {

void SD_UTILITY_API Read(const std::string& file_path, std::string& content);

void SD_UTILITY_API Write(const std::string& file_path, const std::string& content);

}  // namespace File

}  // namespace SD

#endif /* SD_FILE_HPP */
