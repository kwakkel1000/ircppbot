#include "../include/ConfigReader.h"
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>

using namespace std;

ConfigReader::ConfigReader()
{
}


ConfigReader::~ConfigReader()
{
}


bool ConfigReader::ReadFile( string filename )
{
    cout << "readfile: " << filename << endl;
    string line;
    string section("global"); // default value
    ifstream configfile;
    int linenr = 0;

    // Start fresh :)
    settings.clear();

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
                        //cout << "* Changed section to '" << section << "'" << endl;
                    }
                    else
                    {
                        cout << "Invalid section on line " << linenr << ": " << line << endl;
                    }
                }
                else
                {
                    uint pos = line.find("=");
                    if (pos!=string::npos)
                    {
                        string var = line.substr( 0, pos );
                        string value = line.substr( pos+1, line.length()-pos-1);
                        boost::trim(var);
                        boost::trim(value);

                        //cout << "* The variable '" << var << "' has value '" << value << "'" << endl;
                        settings[var] = value;
                    }
                }
            }

        }
        configfile.close();
        return true;
    }
    else
    {
        cout << "Could not open file '" << filename << "'" << endl;
    }

    return false;
}


string ConfigReader::GetString( string varname )
{
    return settings[varname];
}
