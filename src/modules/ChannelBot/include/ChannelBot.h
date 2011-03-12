#ifndef ChannelBot_h
#define ChannelBot_h

#include "../../../include/interfaces/ModuleInterface.h"
#include "../../../include/core/Data.h"
#include "../../../include/core/ModuleBase.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

using namespace std;

class Data;
class ChannelBot : public ModuleBase
{
public:
    ChannelBot();
    ~ChannelBot();
    void read();
    void stop();
    void Init();
    void timerrun();


private:

    Data * D;
    void BindInit();

    void parse_raw();
    void parse_privmsg();
    void ParseData(std::vector< std::string > data);
    void ParsePrivmsg(std::string nick, std::string command, std::string chan, std::vector< std::string > args, int chantrigger);
    void PING(vector<string> data);
    //void PRIVMSG(vector<string> data);
    void INVITE(vector<string> data);
    void WHOIS(vector<string> data);
    void JOIN(vector<string> data);
    void PART(vector<string> data);
    void KICK(vector<string> data);
    void NICK(vector<string> data);
    void MODE(vector<string> data);
    void QUIT(vector<string> data);

    void adduser(string chan, string nick, string auth, string reqnick, string reqauth, int reqaccess, int ca);
    void deluser(string chan, string nick, string auth, string reqnick, string reqauth, int ca);
    void changelevel(string chan, string nick, string auth, string reqnick, string reqauth, int reqaccess, int ca);
    void access(string chan, string nick, string reqnick, string reqauth, int ca);
    void myaccess(string nick, string reqnick, string reqauth, int ca);
    void ping(string chan, string nick, int ca);
    void version(string chan, string nick, int ca);
    void authme(string nick, int ca);
    void up(string chan, string nick, string auth, int ca);
    void down(string chan, string nick, string auth, int ca);
    void resync(string chan, string nick, string auth, int ca);
    void users(string chan, string nick, int ca);
    void kickuser(string chan, string nick, string auth, string reqnick, string reqauth, string reason, int ca);

    void op(string chan, string nick, string auth, string reqnick, string reqauth, int ca);
    void deop(string chan, string nick, string auth, string reqnick, string reqauth, int ca);
    void voice(string chan, string nick, string auth, string reqnick, string reqauth, int ca);
    void devoice(string chan, string nick, string auth, string reqnick, string reqauth, int ca);

    void ccommands(string nick, string auth, int ca);

    void OnUserJoin(string chan, string nick);

    void DBChannelInfo(string data);
    void DBreplyinit();

    void timerlong();
    vector<int> timer_sec;
    vector<string> timer_command;
    vector<int> timer_long_sec;
    vector<string> timer_long_command;
    int longtime;

    vector<string> reply_name_vector;
    vector<string> reply_vector;
    vector<string> reply_language_vector;

    //binds
    vector<string> commands;
    vector<string> binds;
    vector<int> cas;

    boost::mutex parse_mutex;
    bool run;
    std::string channelbottrigger;
    //vector<string> Data;

    boost::shared_ptr<boost::thread> raw_parse_thread;
    boost::shared_ptr<boost::thread> privmsg_parse_thread;
};

#endif // ChannelBot_h
