#ifndef ConfigReader_h
#define ConfigReader_h

#include <string>
#include <map>

using namespace std;

class ConfigReader
{
public:
    ConfigReader();
    ~ConfigReader();

    bool ReadFile( string filename );
    string GetString( string varname );

private:
    map<string,string> settings;

};

#endif // ConfigReader_h
