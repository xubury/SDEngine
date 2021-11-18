#ifndef SD_INI_PARSER_HPP
#define SD_INI_PARSER_HPP

#include "Utility/Base.hpp"

#include <set>
#include <map>

namespace SD {

class SD_API IniParser {
   public:
    // Construct IniParser and parse given filename.
    IniParser(const std::string& filename);
    ~IniParser();

    // Return the result of ParseStream(), i.e., 0 on success, line number of
    // first error on parse error, or -1 on file open error.
    int GetError() const;

    // Return the list of sections found in ini file
    const std::set<std::string>& GetSections() const;

    // Get a string value from INI file, returning default_value if not found.
    std::string Get(const std::string& section, const std::string& name,
                    const std::string& default_value) const;

    // Get an integer (long) value from INI file, returning default_value if
    // not found or not a valid integer (decimal "1234", "-1234", or hex
    // "0x4d2").
    long GetInteger(const std::string& section, const std::string& name,
                    long default_value) const;

    // Get a real (floating point double) value from INI file, returning
    // default_value if not found or not a valid floating point value
    // according to strtod().
    double GetReal(const std::string& section, const std::string& name,
                   double default_value) const;

    // Get a single precision floating point number value from INI file,
    // returning default_value if not found or not a valid floating point value
    // according to strtof().
    float GetFloat(const std::string& section, const std::string& name,
                   float default_value) const;

    // Get a boolean value from INI file, returning default_value if not found
    // or if not a valid true/false value. Valid true values are "true", "yes",
    // "on", "1", and valid false values are "false", "no", "off", "0" (not case
    // sensitive).
    bool GetBoolean(const std::string& section, const std::string& name,
                    bool default_value) const;

    void Set(const std::string& section, const std::string& name,
             const std::string& value) const;

    long SetInteger(const std::string& section, const std::string& name,
                    long value) const;

    double SetReal(const std::string& section, const std::string& name,
                   double value) const;

    float SetFloat(const std::string& section, const std::string& name,
                   float value) const;

    bool SetBoolean(const std::string& section, const std::string& name,
                    bool value) const;

   protected:
    int ParseStream(const std::ifstream& file);
    int ValueHandler(const std::string& section, const std::string& name,
                     const std::string& value);
    static std::string MakeKey(const std::string& section,
                               const std::string& name);

   private:
    std::string m_filenname;
    int m_error;
    std::map<std::string, std::string> m_values;
    std::set<std::string> m_sections;
};

}  // namespace SD

#endif
