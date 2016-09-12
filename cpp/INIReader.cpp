// Read an INI file into easy-to-access name/value pairs.

// inih and INIReader are released under the New BSD license (see LICENSE.txt).
// Go to the project home page for more info:
//
// https://github.com/benhoyt/inih

#include "INIReader.h"
#include "../ini.h"
#ifdef MASTER_APPROACH
#include <algorithm>
#else
#include <regex>
#endif

INIReader::INIReader(const std::string& filename) :
    _error(0),
    _values()
{
    _error = ini_parse(filename.c_str(), ValueHandler, this);
}

int INIReader::ParseError() const
{
    return _error;
}

std::string INIReader::Get(const std::string& section, const std::string& name, const std::string& default_value) const
{
    const auto key = MakeKey(section, name);
    // Use _values.find() here instead of _values.at() to support pre C++11 compilers
    return _values.count(key) ? _values.find(key)->second : default_value;
}

long INIReader::GetInteger(const std::string& section, const std::string& name, long default_value) const
{
#ifdef MASTER_APPROACH
    const auto valstr = Get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    // This parses "1234" (decimal) and also "0x4D2" (hex)
    long n = strtol(value, &end, 0);
    return end > value ? n : default_value;
#else
    const auto valstr = Get(section, name, "");
    long n = 0;
    try {
        // This parses "1234" (decimal) and also "0x4D2" (hex)
        n = std::stol(valstr);
    }
    catch (...) {
        n = default_value;
    }
    return n;
#endif
}

double INIReader::GetReal(const std::string& section, const std::string& name, double default_value) const
{
#ifdef MASTER_APPROACH
    const auto valstr = Get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    double n = strtod(value, &end);
    return end > value ? n : default_value;
#else
    const auto valstr = Get(section, name, "");
    double n = 0.0;
    try {
        n = std::stod(valstr);
    }
    catch (...) {
        n = default_value;
    }
    return n;
#endif
}

bool INIReader::GetBoolean(const std::string& section, const std::string& name, bool default_value) const
{
#ifdef MASTER_APPROACH
    auto valstr = Get(section, name, "");
    // Convert to lower case to make string comparisons case-insensitive
    std::transform(valstr.begin(), valstr.end(), valstr.begin(), ::tolower);
    if (valstr == "true" || valstr == "yes" || valstr == "on" || valstr == "1")
        return true;
    else if (valstr == "false" || valstr == "no" || valstr == "off" || valstr == "0")
        return false;
    else
        return default_value;
#else
    static const auto true_pattern = std::regex("[Tt][Rr][Uu][Ee]||[Yy][Ee][Ss]||[Oo][Nn]||[1]");
    static const auto false_pattern = std::regex("[Ff][Aa][Ll][Ss][Ee]||[Nn][Oo]||[Oo][Ff][Ff]||[0]");
    const auto valstr = Get(section, name, "");

    std::smatch smatch;
    if (std::regex_match(valstr, smatch, true_pattern))
        return true;
    else if (std::regex_match(valstr, smatch, false_pattern))
        return false;
    else
        return default_value;
#endif
}

std::string INIReader::MakeKey(const std::string& section, const std::string& name)
{
    auto key = section + "=" + name;
    // Convert to lower case to make section/name lookups case-insensitive
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    return key;
}

int INIReader::ValueHandler(void* user, const char* section, const char* name,
                            const char* value)
{
    auto* reader = reinterpret_cast<INIReader*>(user);
    const auto key = MakeKey(section, name);
    if (!reader->_values[key].empty())
        reader->_values[key] += "\n";
    reader->_values[key] += value;
    return 1;
}
