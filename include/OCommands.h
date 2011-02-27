#ifndef Tran_OCommands_h
#define Tran_OCommands_h

#include "ModuleInterface.h"
#include "ModuleBase.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

using namespace std;

class OCommands : public ModuleBase
{
public:
    OCommands();
    ~OCommands();
    void threadloop();
    void stopthreadloop();
    void Init();
    void timerrun();

private:
    void BindInit();

    void ParsePrivmsg(std::vector<std::string> data, std::string command, std::string chan, std::vector< std::string > args, int chantrigger);


    //overwatch
    void overwatch(string bind, string command, string chan, string nick, string auth, vector<string> args);
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


    boost::condition_variable_any parse_wait_condition;
    boost::mutex parse_mutex;
    bool data_ready;
    bool runthreadloop;
    //vector<string> Data;
};

#endif // Tran_OCommands_h

