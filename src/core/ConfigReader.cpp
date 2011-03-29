#include "../include/core/ConfigReader.h"
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>

//using namespace std;

ConfigReader::ConfigReader()
{
	ClearSettings();
}


ConfigReader::~ConfigReader()
{
}

void ConfigReader::ClearSettings()
{
    settings.clear();
}


bool ConfigReader::ReadFile( std::string filename )
{
    std::cout << "readfile: " << filename << std::endl;
    std::string line;
    std::string section("global"); // default value
    std::ifstream configfile;
    int linenr = 0;

    configfile.open(filename.c_str());
    if (configfile.is_open())
    {
        while (configfile.good())
        {
            getline(configfile,line);
            linenr++;

            // Uncomment if boost available :D
            boost::trim(line);

            // Filter empty and comment lines
            if ((line.length()>0) && (line[0] != '#'))
            {
                // Debug
                //cout << "Line " << linenr << ": " << line << endl;

                // Detect section begin
                if (line[0] == '[')
                {
                    if (line[line.length()-1] == ']')
                    {
                        // Changing section...
                        section = line.substr( 1, line.length()-2 );
                        std::cout << "* Changed section to '" << section << "'" << std::endl;
                    }
                    else
                    {
                        std::cout << "Invalid section on line " << linenr << ": " << line << std::endl;
                    }
                }
                else
                {
                	if (boost::iequals(section, "config"))
                	{
						std::string var = line;
						boost::trim(var);
                		ReadFile(var);
                	}
                	else
                	{
						uint pos = line.find("=");
						if (pos!=std::string::npos)
						{
							std::string var = line.substr( 0, pos );
							std::string value = line.substr( pos+1, line.length()-pos-1);
							boost::trim(var);
							boost::trim(value);

							//cout << "* The variable '" << var << "' has value '" << value << "'" << endl;
							settings[var] = value;
						}
                	}
                }
            }
        }
        configfile.close();
		std::cout << "done reading: " << filename << std::endl;
        return true;
    }
    else
    {
        std::cout << "Could not open file '" << filename << "'" << std::endl;
    }

    return false;
}


std::string ConfigReader::GetString( std::string varname )
{
    return settings[varname];
}
