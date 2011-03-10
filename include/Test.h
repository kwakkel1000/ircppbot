#ifndef Test_H
#define Test_H
#include "ModuleInterface.h"
#include "ModuleBase.h"
#include "Data.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

class Data;
class Test : public ModuleBase
{
public:
    Test();
    ~Test();
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
};

#endif // Test_H

