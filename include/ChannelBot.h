#ifndef Tran_ChannelBot_h
#define Tran_ChannelBot_h

#include "ModuleInterface.h"
#include "ModuleBase.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

using namespace std;

class ChannelBot : public ModuleBase
{
public:
    ChannelBot();
    ~ChannelBot();
    void threadloop();
    void stopthreadloop();
    void Init();
    void timerrun();


private:
    void BindInit();

    void ParsePrivmsg(std::vector<std::string> data, std::string command, std::string chan, std::vector< std::string > args, int chantrigger);
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

    void overwatch(string bind, string command, string chan, string nick, string auth, vector<string> args);

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

    vector<string> lineout(vector<string> data, unsigned int rowamount, unsigned int length);
    string convertInt(int);
    int convertString(string);
    void DBChannelInfo(string data);
    bool Send(string data);
    vector< vector<string> > RawSqlSelect(string data);
    bool RawSql(string data);
    string HostmaskToNick(vector<string> data);
    void DBreplyinit();
    string irc_reply(string reply_name, string reply_language);
    string irc_reply_replace(string source_string, string search_string, string replace_string);

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

    bool caseInsensitiveStringCompare( const std::string& str1, const std::string& str2 );
    vector<string> Split(const std::string&, const std::string&, bool, bool);

    boost::condition_variable_any parse_wait_condition;
    boost::mutex parse_mutex;
    bool data_ready;
    bool runthreadloop;
    //vector<string> Data;
};

#endif // Tran_ChannelBot_h
