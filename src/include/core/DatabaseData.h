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
    DatabaseData();
    ~DatabaseData();

    void init();

private:
    //vars

    //config vars
    std::string mHostName;
    std::string mDatabaseName;
    std::string mUserName;
    std::string mPass;

    //thread vars
    boost::shared_ptr< boost::thread > mQueryThread;

    //functions
    std::vector< std::vector< std::string > > RawSqlSelect(std::string data);
    std::string convertInt(int);
    int convertString(std::string);
    void DatabaseInit();
    void QueryRun();
};


#endif // DatabaseData_h
