#ifndef SD_INI_HPP
#define SD_INI_HPP

#include "Utility/Base.hpp"

#include <set>
#include <map>

namespace SD {

class SD_API Ini {
   public:
    Ini() = default;
    virtual ~Ini() = default;

    void Load(const std::string& filename);
    void Save(const std::string& filename) const;

    void ParseStream(std::istream& stream);
    void OutputStream(std::ostream& stream) const;

    static std::string MakeKey(const std::string& section,
                               const std::string& name);

    // Return the list of sections found in ini file
    const std::set<std::string>& GetSections() const { return m_sections; }

    void Set(const std::string& section, const std::string& name,
             const std::string& value);

    void SetInteger(const std::string& section, const std::string& name,
                    int value);

    void SetReal(const std::string& section, const std::string& name,
                 double value);

    void SetFloat(const std::string& section, const std::string& name,
                  float value);

    void SetBoolean(const std::string& section, const std::string& name,
                    bool value);

    // Get a string value from INI file, returning default_value if not found.
    std::string Get(const std::string& section, const std::string& name,
                    const std::string& default_value) const;

    // Get an integer (long) value from INI file, returning default_value if
    // not found or not a valid integer (decimal "1234", "-1234", or hex
    // "0x4d2").
    int GetInteger(const std::string& section, const std::string& name,
                   int default_value) const;

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

   private:
    std::map<std::string, std::string> m_values;
    std::set<std::string> m_sections;
};

}  // namespace SD

#endif
