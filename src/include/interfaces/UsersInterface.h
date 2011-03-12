#ifndef UsersInterface_h
#define UsersInterface_h

#include <string>
#include <vector>

class UsersInterface
{
public:

    virtual bool AddUser(std::string)=0;
    virtual bool DelUser(std::string)=0;

    virtual bool ChangeNick(std::string, std::string)=0;

    virtual bool AddWhois(std::string)=0;
    virtual bool DelWhois(std::string)=0;
    virtual std::string GetWhois()=0;

    virtual void Debug()=0;

    virtual bool SetAuth(std::string, std::string)=0;
    virtual std::string GetAuth(std::string)=0;
    virtual std::vector< std::string> GetNicks(std::string)=0;
    virtual bool AddAuth(std::string)=0;
    virtual std::vector< std::string> GetAuths()=0;

    virtual bool SetOaccess(std::string, int)=0;
    virtual int GetOaccess(std::string)=0;

    virtual bool God(std::string)=0;
    virtual bool SetGod(std::string, int)=0;
    virtual int GetGod(std::string)=0;

    virtual bool AddChannel(std::string, std::string)=0;
    virtual bool DelChannel(std::string, std::string)=0;
    virtual std::vector< std::string > GetChannels(std::string)=0;

    virtual int GetUid(std::string)=0;
    virtual bool SetUid(std::string, int)=0;

    virtual bool SetGone(std::string, bool)=0;
    virtual bool GetGone(std::string)=0;

    virtual bool SetX(std::string, bool)=0;
    virtual bool GetX(std::string)=0;

    virtual bool SetD(std::string, bool)=0;
    virtual bool GetD(std::string)=0;

    virtual bool SetIrcop(std::string, bool)=0;
    virtual bool GetIrcop(std::string)=0;

    virtual bool SetLanguage(std::string, std::string)=0;
    virtual std::string GetLanguage(std::string)=0;

    virtual int GetWidthLength(std::string)=0;
    virtual int GetWidth(std::string)=0;
};

#endif // UsersInterface_h

