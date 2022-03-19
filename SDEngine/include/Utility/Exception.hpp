#ifndef SD_EXCEPTION_HPP
#define SD_EXCEPTION_HPP

#include "Utility/Export.hpp"
#include "Utility/Log.hpp"

#include <exception>
#include <string>

namespace SD {

class SD_UTILITY_API Exception : public std::exception, public std::string {
   public:
    Exception(void) throw() {}

    Exception(const std::string& msg) throw() : std::string(msg) {}

    virtual ~Exception(void) throw() {}

    virtual const char* what(void) const throw() { return c_str(); }
};

class SD_UTILITY_API FileException : public Exception {
   public:
    FileException(const std::string& file_path, const std::string& msg) throw()
        : Exception(fmt::format("File exception from \"{}\", error: {}",
                                file_path, msg))
    {
    }

    virtual ~FileException(void) throw() {}
};

}  // namespace SD

#endif  // !SD_EXCEPTION_HPP
