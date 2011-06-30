#ifndef User_h
#define User_h
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include <boost/thread/mutex.hpp>

using namespace std;

class User
{
public:

    User();
    virtual ~User();

    virtual void SetNick(std::string);
    virtual std::string GetNick();

    virtual void SetAuth(std::string);
    virtual std::string GetAuth();

    virtual bool AddChannel(std::string);
    virtual bool DelChannel(std::string);

    virtual vector< std::string > GetChannels();

    virtual void SetOaccess(int);
    virtual int GetOaccess();

    virtual void God();
    virtual void SetGod(int);
    virtual int GetGod();

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
    std::string nick;
    std::string auth;
    std::vector< std::string > channels;
    std::vector< std::string > NULLvector;
    bool gone;
    bool x;
    bool d;
    bool ircop;
    std::string lang;
    int GetChannelIndex(std::string sChannel);
    int oaccess;
    int god;

    boost::mutex User_mutex;
};

#endif // User_h
