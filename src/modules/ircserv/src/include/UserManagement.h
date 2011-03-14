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
    void ParseData(std::vector< std::string > data);
    void read();
    void stop();
    void Init(DataInterface* pData);


private:
	bool NickServ;
	bool WhoExtra;
	bool Run;
    DataInterface* mpDataInterface;

    void WHO(std::vector< std::string > data);
    void WHOEXTRA(std::vector< std::string > data);
    void WHOIS(std::vector< std::string > data);
    void JOIN(std::vector< std::string > data);
    void PART(std::vector< std::string > data);
    void KICK(std::vector< std::string > data);
    void NICK(std::vector< std::string > data);
    void MODE(std::vector< std::string > data);
    void QUIT(std::vector< std::string > data);
	void UserAuth(std::string mNick, std::string mAuth);

    std::string HostmaskToNick(std::vector< std::string > data);
    bool Send(std::string data);
    std::vector< std::vector< std::string > > RawSqlSelect(std::string data);
    bool RawSql(std::string data);
    std::string convertInt(int);
    int convertString(std::string);
    void DBChannelInfo(std::string data);
    void DBUserInfo(std::string data);
    void DBinit();

    void parse_raw();
    boost::shared_ptr<boost::thread> raw_parse_thread;

    bool caseInsensitiveStringCompare( const std::string& str1, const std::string& str2 );
    std::vector< std::string > Split(const std::string&, const std::string&, bool, bool);
};


#endif // UserManagement_h
