#ifndef Support_H
#define Support_H

#include "ModuleInterface.h"
#include "ModuleBase.h"
#include "Data.h"
#include <string>
#include <vector>

class Data;
class Support : public ModuleBase
{
public:
	Support();
	~Support();
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
    void support(std::string nick, std::string auth, std::string supportstring);

    void timerlong();
    std::vector<int> timer_sec;
    std::vector< std::string > timer_command;
    std::vector<int> timer_long_sec;
    std::vector< std::string > timer_long_command;
    int longtime;

    bool run;
    boost::shared_ptr<boost::thread> raw_parse_thread;
    boost::shared_ptr<boost::thread> privmsg_parse_thread;


    std::map< std::string, std::string > support_nick_string_map;
    std::map< std::string, std::string >::iterator support_nick_string_it;
    std::pair< std::map< std::string, std::string >::iterator, bool > support_ret;
    std::vector< std::string > support_nick;
};

#endif // Support_H


