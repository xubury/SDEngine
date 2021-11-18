#ifndef SD_FILE_HPP
#define SD_FILE_HPP

#include <string>
#include "Utility/Export.hpp"

namespace SD {

class Exception : public std::exception, public std::string {
   public:
    Exception(void) throw() {}

    Exception(const std::string& msg) throw() : std::string(msg) {}

    virtual ~Exception(void) throw() {}

    virtual const char* what(void) const throw() { return c_str(); }
};

class FileException : public Exception {
   public:
    FileException(const std::string& file_path, const std::string& msg) throw()
        : Exception(msg), m_file_path(file_path) {}

    virtual ~FileException(void) throw() {}

    const std::string& GetFilePath() const { return m_file_path; }

   public:
    std::string m_file_path;
};

namespace File {

void SD_API Read(const std::string& file_path, std::string& content);

void SD_API Write(const std::string& file_path, const std::string& content);

}  // namespace File

}  // namespace SD

#endif /* SD_FILE_HPP */
