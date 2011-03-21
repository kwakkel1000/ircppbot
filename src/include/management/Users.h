#ifndef Users_h
#define Users_h
#include "../interfaces/UsersInterface.h"

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "User.h"
#include <boost/thread/mutex.hpp>

class Users : public UsersInterface
{
public:
    Users();
    virtual ~Users();

    virtual bool AddUser(std::string);
    virtual bool DelUser(std::string);

    virtual bool ChangeNick(std::string, std::string);

    virtual bool AddWhois(std::string);
    virtual bool DelWhois(std::string);
    virtual std::string GetWhois();

    virtual void Debug();

    virtual bool SetAuth(std::string, std::string);
    virtual std::string GetAuth(std::string);
    virtual std::vector< std::string> GetNicks(std::string);
    virtual bool AddAuth(std::string);
    virtual std::vector< std::string> GetAuths();

    virtual bool SetOaccess(std::string, int);
    virtual int GetOaccess(std::string);

    virtual bool God(std::string);
    virtual bool SetGod(std::string, int);
    virtual int GetGod(std::string);

    virtual bool AddChannel(std::string, std::string);
    virtual bool DelChannel(std::string, std::string);
    virtual std::vector< std::string > GetChannels(std::string);

    virtual std::string GetUid(std::string);
    virtual bool SetUid(std::string, std::string);

    virtual bool SetGone(std::string, bool);
    virtual bool GetGone(std::string);

    virtual bool SetX(std::string, bool);
    virtual bool GetX(std::string);

    virtual bool SetD(std::string, bool);
    virtual bool GetD(std::string);

    virtual bool SetIrcop(std::string, bool);
    virtual bool GetIrcop(std::string);

    virtual bool SetLanguage(std::string, std::string);
    virtual std::string GetLanguage(std::string);

    virtual int GetWidthLength(std::string);

    virtual int GetWidth(std::string);

private:
    std::vector< std::string > nick;
    std::vector< User* > u;
    std::vector< std::string > whoislist;
    std::vector< std::string > NULLvector;
    std::vector< std::string > authlist;

    /*boost::mutex User_mutex;
    boost::mutex UsersGetNicks_mutex;
    boost::mutex UsersGetAuths_mutex;
    boost::mutex UsersGetChannels_mutex;
    boost::mutex UsersWhois_mutex;*/

    int GetNickIndex(std::string data);
    int GetWhoisIndex(std::string data);
};

#endif // Users_h
