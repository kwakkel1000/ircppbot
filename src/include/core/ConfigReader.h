#ifndef ConfigReader_h
#define ConfigReader_h

#include "../interfaces/ConfigReaderInterface.h"

#include <string>
#include <map>

class ConfigReader : public ConfigReaderInterface
{
public:
    ConfigReader();
    ~ConfigReader();

    void ClearSettings();

    bool ReadFile( std::string filename );
    std::string GetString( std::string varname );

private:
    std::map< std::string, std::string > settings;

};

#endif // ConfigReader_h
