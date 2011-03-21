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

    virtual bool SetUid(std::string);
    virtual std::string GetUid();

    virtual bool SetGone(bool);
    virtual bool GetGone();

    virtual bool SetX(bool);
    virtual bool GetX();

    virtual bool SetD(bool);
    virtual bool GetD();

    virtual bool SetIrcop(bool);
    virtual bool GetIrcop();

    virtual bool SetLanguage(std::string);
    virtual std::string GetLanguage();



private:
    string nick;
    string auth;
    vector<string> channels;
    vector<string> NULLvector;
    bool gone;
    bool x;
    bool d;
    bool ircop;
    std::string lang;
    std::string uid;
    int GetChannelIndex(string data);
    int oaccess;
    int god;
};

#endif // User_h
