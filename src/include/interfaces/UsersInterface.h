#ifndef UsersInterface_h
#define UsersInterface_h

#include <string>
#include <vector>

class UsersInterface
{
public:
    virtual void Debug()=0;										//Modules

    virtual std::string GetAuth(std::string)=0;					//Modules
    virtual std::vector< std::string> GetNicks(std::string)=0;	//Modules	//new function needed: GetAuthNicks

    virtual bool SetOaccess(std::string, int)=0;				//Modules
    virtual int GetOaccess(std::string)=0;						//Modules

    virtual bool God(std::string)=0;
    virtual bool SetGod(std::string, int)=0;					//Modules
    virtual int GetGod(std::string)=0;							//Modules

    virtual bool DelChannel(std::string, std::string)=0;		//Modules	//new function needed: dont know wich yet

    virtual bool SetLanguage(std::string, std::string)=0;		//Modules
    virtual std::string GetLanguage(std::string)=0;				//Modules

    virtual int GetWidthLength(std::string)=0;					//Modules
    virtual int GetWidth(std::string)=0;						//Modules


    virtual std::string GetUid(std::string)=0;


    friend class UserManagement;

private:
    virtual bool AddUser(std::string)=0;
    virtual bool DelUser(std::string)=0;

	virtual bool ChangeNick(std::string, std::string)=0;

	virtual bool AddChannel(std::string, std::string)=0;
    virtual std::vector< std::string > GetChannels(std::string)=0;

	virtual bool AddAuth(std::string)=0;
    virtual bool SetAuth(std::string, std::string)=0;

    virtual bool SetUid(std::string, std::string)=0;

	virtual bool SetGone(std::string, bool)=0;

    virtual bool SetX(std::string, bool)=0;

    virtual bool SetD(std::string, bool)=0;

    virtual bool SetIrcop(std::string, bool)=0;

};

#endif // UsersInterface_h

