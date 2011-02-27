#ifndef Users_h
#define Users_h

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "User.h"
#include <boost/thread/mutex.hpp>

using namespace std;

class Users
{
public:
    Users();
    virtual ~Users();

    virtual bool AddUser(string);
    virtual bool DelUser(string);

    virtual bool ChangeNick(string, string);

    virtual bool AddWhois(string);
    virtual bool DelWhois(string);
    virtual string GetWhois();

    virtual void Debug();

    virtual bool SetAuth(string, string);
    virtual string GetAuth(string);
    virtual vector<string> GetNicks(string);
    virtual bool AddAuth(string);
    virtual vector<string> GetAuths();

    virtual bool SetOaccess(string, int);
    virtual int GetOaccess(string);

    virtual bool God(string);
    virtual bool SetGod(string, int);
    virtual int GetGod(string);

    virtual bool AddChannel(string, string);
    virtual bool DelChannel(string, string);
    virtual vector<string> GetChannels(string);

    virtual int GetUid(string);
    virtual bool SetUid(string, int);

    virtual bool SetGone(string, bool);
    virtual bool GetGone(string);

    virtual bool SetX(string, bool);
    virtual bool GetX(string);

    virtual bool SetD(string, bool);
    virtual bool GetD(string);

    virtual bool SetIrcop(string, bool);
    virtual bool GetIrcop(string);

    virtual string GetLanguage(string);

    virtual int GetWidthLength(string);

    virtual int GetWidth(string);

private:
    vector<string> nick;
    vector<User*> u;
    vector<string> whoislist;
    vector<string> NULLvector;
    vector<string> authlist;

    boost::mutex User_mutex;
    boost::mutex UsersGetNicks_mutex;
    boost::mutex UsersGetAuths_mutex;
    boost::mutex UsersGetChannels_mutex;
    boost::mutex UsersWhois_mutex;

    int GetNickIndex(string data);
    int GetWhoisIndex(string data);
    bool caseInsensitiveStringCompare( const std::string& str1, const std::string& str2 );
};

#endif // Users_h
