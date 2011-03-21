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

	std::string GetUserUuidByAuth(std::string auth);
	int GetOaccessByAuth(std::string auth);
	int GetGodByAuth(std::string auth);
	std::string GetLanguageByAuth(std::string auth);

private:
    DatabaseData();
    ~DatabaseData();

    //vars
    bool mRun;
    std::queue< std::string > sql_queue;

    //database_results
    std::vector< std::vector< std::string > > auth_vector;

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
