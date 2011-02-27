#ifndef Support_H
#define Support_H

#include "ModuleInterface.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <boost/thread/condition.hpp>

using namespace std;

class Support : public ModuleInterface
{
public:
    // No constructor/destructor? :p

    void ParseData(vector<string> data);
    void threadloop();
    void stopthreadloop();
    void Init(string nick, IrcSocket *s, Users *u, Channels *c, ConfigReader& reader, IrcData *id);
    void timerrun();

private:
    void PRIVMSG(vector<string> data);
    bool Send(string data);
    void overwatch(string bind, string command, string chan, string nick, string auth, vector<string> args);
    void support(string nick, string auth, string supportstring);

    string convertInt(int);
    int convertString(string);
    string HostmaskToNick(vector<string> data);
    vector<string> lineout(vector<string> data, unsigned int rowamount, unsigned int length);
    bool caseInsensitiveStringCompare( const std::string& str1, const std::string& str2 );
    vector<string> Split(const std::string&, const std::string&, bool, bool);
    void BindInit();
    vector< vector<string> > RawSqlSelect(string data);
    bool RawSql(string data);
    void DBreplyinit();
    string irc_reply(string reply_name, string reply_language);
    string irc_reply_replace(string source_string, string search_string, string replace_string);

    vector<string> reply_name_vector;
    vector<string> reply_vector;
    vector<string> reply_language_vector;


    //binds
    vector<string> commands;
    vector<string> binds;
    vector<int> oas;


    string supportchannel;
    boost::condition_variable parse_wait_condition;
    boost::mutex parse_mutex;
    bool data_ready;
    bool runthreadloop;
    vector<string> Data;


    map<string, string> support_nick_string_map;
    map<string, string>::iterator support_nick_string_it;
    pair<map<string, string>::iterator, bool> support_ret;
    vector<string> support_nick;
};

#endif // Support_H

