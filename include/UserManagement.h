#ifndef UserManagement_h
#define UserManagement_h


#include "UserManagementInterface.h"
#include "ConfigReader.h"
#include <string>
#include <vector>

using namespace std;

class database;
class ConfigReader;
class UserManagement : public UserManagementInterface
{
public:
    UserManagement();

    virtual void ParseData(vector<string> data)=0;
    void Init(string nick, IrcSocket *s, Users *u, Channels *c, ConfigReader& reader, IrcData *id);

    void WHO(vector<string> data);
    void WHOIS(vector<string> data);
    void JOIN(vector<string> data);
    void PART(vector<string> data);
    void KICK(vector<string> data);
    void NICK(vector<string> data);
    void MODE(vector<string> data);
    void QUIT(vector<string> data);

    string HostmaskToNick(vector<string> data);
    bool Send(string data);
    vector< vector<string> > RawSqlSelect(string data);
    bool RawSql(string data);
    string convertInt(int);
    int convertString(string);
    void DBChannelInfo(string data);
    void DBUserInfo(string data);
    void DBinit();

    bool caseInsensitiveStringCompare( const std::string& str1, const std::string& str2 );
    vector<string> Split(const std::string&, const std::string&, bool, bool);
};


#endif // UserManagement_h
