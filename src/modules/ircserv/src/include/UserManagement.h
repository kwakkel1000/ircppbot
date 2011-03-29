#ifndef UserManagement_h
#define UserManagement_h

#include <interfaces/UserManagementInterface.h>
#include <interfaces/DataInterface.h>
#include <string>
#include <vector>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

class DataInterface;
class UserManagement : public UserManagementInterface
{
public:
	UserManagement();
	~UserManagement();
    void ParseData(std::vector< std::string > data);
    void read();
    void stop();
    void Init(DataInterface* pData);


private:
	bool NickServ;
	//bool WhoExtra;
	bool Run;
    DataInterface* mpDataInterface;

    void WHO(std::vector< std::string > data);
    void WHOEXTRA(std::vector< std::string > data);
    void JOIN(std::vector< std::string > data);
    void PART(std::vector< std::string > data);
    void KICK(std::vector< std::string > data);
    void NICK(std::vector< std::string > data);
    void MODE(std::vector< std::string > data);
    void QUIT(std::vector< std::string > data);

    std::string HostmaskToNick(std::vector< std::string > data);
	void UserAuth(std::string mNick, std::string mAuth);
    bool Send(std::string data);

    std::string convertInt(int);
    int convertString(std::string);

    void GetChannelInfo(std::string data);
    void GetUserInfo(std::string data);
    void GetAuths();

    void parse_raw();
    boost::shared_ptr<boost::thread> raw_parse_thread;
};


#endif // UserManagement_h
