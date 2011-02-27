#ifndef User_h
#define User_h
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

class User
{
public:


    User();


    virtual ~User();



    virtual void SetNick(string);


    virtual string GetNick();


    virtual void SetAuth(string);


    virtual string GetAuth();


    virtual bool AddChannel(string);


    virtual bool DelChannel(string);


    virtual vector<string> GetChannels();



    virtual void SetOaccess(int);
    virtual int GetOaccess();

    virtual void God();
    virtual void SetGod(int);
    virtual int GetGod();

    virtual bool SetUid(int);
    virtual int GetUid();

    virtual bool SetGone(bool);
    virtual bool GetGone();

    virtual bool SetX(bool);
    virtual bool GetX();

    virtual bool SetD(bool);
    virtual bool GetD();

    virtual bool SetIrcop(bool);
    virtual bool GetIrcop();



private:
    string nick;
    string auth;
    vector<string> channels;
    vector<string> NULLvector;
    bool gone;
    bool x;
    bool d;
    bool ircop;
    int uid;
    int GetChannelIndex(string data);
    bool caseInsensitiveStringCompare( const std::string& str1, const std::string& str2 );
    int oaccess;
    int god;
};

#endif // User_h
