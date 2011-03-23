#ifndef DatabaseData_h
#define DatabaseData_h

#include <string>
#include <vector>
#include <queue>

#include <boost/bind.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
class DatabaseData
{
public:
	static DatabaseData& Instance()
	{
		static DatabaseData obj;
		return obj;
	}

    void init();
    void DatabaseInit();

    void AddAuth(std::string mUserUuid, std::string mAuth);

	void AddChannel(std::string mChannelUuid, std::string mChannel);
	void DeleteChannel(std::string mChannelUuid);

	void AddUserToChannel(std::string mChannelUuid, std::string mUserUuid, int mAccess);
	void DeleteUserFromChannel(std::string mChannelUuid, std::string mUserUuid);

	std::string GetUserUuidByAuth(std::string auth);
	std::string GetAuthByUserUuid(std::string UserUuid);
	int GetOaccessByAuth(std::string auth);
	int GetGodByAuth(std::string auth);
	std::string GetLanguageByAuth(std::string auth);

	std::string GetChannelUuidByChannel(std::string channel);
	std::string GetChannelByChannelUuid(std::string ChannelUuid);
	int GetGiveOpsByChannel(std::string channel);
	int GetGiveVoiceByChannel(std::string channel);

	std::vector< std::vector< std::string > > GetUserUuidAndAccessByChannelUuid(std::string ChannelUuid);
	std::vector< std::string > GetAuths();

private:
    DatabaseData();
    ~DatabaseData();

    //vars
    bool mRun;
    std::queue< std::string > sql_queue;

    //database_results
    std::vector< std::vector< std::string > > auth_vector;
	std::vector< std::vector< std::string > > channels_vector;
	std::vector< std::vector< std::string > > users_vector;

    //config vars
    std::string mHostName;
    std::string mDatabaseName;
    std::string mUserName;
    std::string mPass;

    //thread vars
    boost::shared_ptr< boost::thread > mQueryThread;
    boost::condition SqlAvailable;
    boost::mutex SqlMutex;

    //functions
    std::vector< std::vector< std::string > > RawSqlSelect(std::string data);
    std::string convertInt(int);
    int convertString(std::string);
    void QueryRun();
};


#endif // DatabaseData_h
