#ifndef ConfigReaderInterface_h
#define ConfigReaderInterface_h
#include <string>
class ConfigReaderInterface
{
public:
	virtual void ClearSettings()=0;
    virtual bool ReadFile( std::string filename )=0;
    virtual std::string GetString( std::string varname )=0;
};

#endif // ConfigReaderInterface_h

