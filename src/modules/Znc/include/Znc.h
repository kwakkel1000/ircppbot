#ifndef Znc_H
#define Znc_H

#include "../../../include/interfaces/ModuleInterface.h"
#include "../../../include/core/Data.h"
#include "../../../include/core/ModuleBase.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>

class Data;
class Znc : public ModuleBase
{
public:
    Znc();
    ~Znc();
    void read();
    void stop();
    void Init();
    void timerrun();

private:
    Data * D;

    void parse_raw();
    void parse_privmsg();
    void ParseData(std::vector< std::string > data);
    void ParsePrivmsg(std::vector<std::string> data, std::string command, std::string chan, std::vector< std::string > args, int chantrigger);

    void timerlong();
    std::vector<int> timer_sec;
    std::vector< std::string > timer_command;
    std::vector<int> timer_long_sec;
    std::vector< std::string > timer_long_command;
    int longtime;

    bool run;
    boost::shared_ptr<boost::thread> raw_parse_thread;
    boost::shared_ptr<boost::thread> privmsg_parse_thread;


    std::map< std::string, std::map< std::string, std::string > > znc_user_setting_map;
    std::map< std::string, std::map< std::string, std::string > >::iterator znc_user_setting_it;
    std::vector< std::string > znc_user_nick;
    bool ReadFile( std::string filename );
};

#endif // Znc_H


