#ifndef OCommands_h
#define OCommands_h

#include "../../../include/interfaces/ModuleInterface.h"
#include "../../../include/core/Data.h"
#include "../../../include/core/ModuleBase.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

using namespace std;

class Data;
class OCommands : public ModuleBase
{
public:
    OCommands();
    ~OCommands();
    void read();
    void stop();
    void Init();
    void timerrun();

private:

    Data * D;
    void BindInit();

    //void parse_raw();
    void parse_privmsg();
    void ParseData(std::vector< std::string > data);
    void ParsePrivmsg(std::string nick, std::string command, std::string chan, std::vector< std::string > args, int chantrigger);

    //functions
    void god(string nick, string auth, int oa);
    void addchannel(string chan, string nick, string auth, string reqchan, string reqnick, string reqauth, int oa);
    void delchannel(string chan, string nick, string auth, string reqchan, int oa);
    void addobind(string nick, string auth, string command, string newbind, int reqaccess, int oa);
    void delobind(string nick, string auth, string command, string newbind, int oa);
    void addbind(string nick, string auth, string command, string newbind, int reqaccess, int oa);
    void delbind(string nick, string auth, string command, string newbind, int oa);
    void addouser(string nick, string auth, string reqnick, string reqauth, int reqaccess, int oa);
    void delouser(string nick, string auth, string reqnick, string reqauth, int oa);
    void changeolevel(string nick, string auth, string reqnick, string reqauth, int reqaccess, int oa);
    void ousers(string nick, int oa);
    void say(string chan, string nick, string auth, string saystring, int oa);
    void raw(string nick, string auth, string dostring, int oa);
    void ocommands(string nick, string auth, int oa);

    void DBChannelInfo(string data);

    vector<string> commands;
    vector<string> binds;
    vector<int> oas;

    vector<string> ccommands;
    vector<string> cbinds;
    vector<int> cas;


    boost::mutex parse_mutex;
    bool run;
    std::string ocommandstrigger;
    //vector<string> Data;
    //boost::shared_ptr<boost::thread> raw_parse_thread;
    boost::shared_ptr<boost::thread> privmsg_parse_thread;
};

#endif // OCommands_h

