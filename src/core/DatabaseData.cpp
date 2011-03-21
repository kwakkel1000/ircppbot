#include "../include/core/DatabaseData.h"
#include "../include/core/Database.h"
#include "../include/core/Global.h"
#include "../include/interfaces/ConfigReaderInterface.h"
#include <boost/algorithm/string.hpp>
#include <iostream>

DatabaseData::DatabaseData()
{
    assert(!mQueryThread);
    mQueryThread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&DatabaseData::QueryRun, this)));
}

DatabaseData::~DatabaseData()
{

}

void DatabaseData::init()
{
    Global& G = Global::Instance();
	ConfigReaderInterface& reader = G.get_ConfigReader();
    mHostName = reader.GetString("hostname");
	mDatabaseName = reader.GetString("databasename");
    mUserName = reader.GetString("username");
    mPass = reader.GetString("password");
    mRun = true;
}

void DatabaseData::DatabaseInit()
{
    std::string sql_string;
	auth_vector.clear();

    sql_string = "select UserUuid, auth, oaccess, god, language from auth";
    auth_vector = RawSqlSelect(sql_string);
}

void DatabaseData::AddAuth(std::string mUserUuid, std::string mAuth)
{
	std::string sqlstring = "INSERT into auth (UserUuid, auth) VALUES ( '" + mUserUuid + "', '" + mAuth + "' );";
	std::cout << sqlstring << std::endl;
    //boost::mutex::scoped_lock lock(SqlMutex);
    sql_queue.push(sqlstring);
    SqlAvailable.notify_one();
}

std::string DatabaseData::GetUserUuidByAuth(std::string auth)
{
    for ( unsigned int i = 0 ; i < auth_vector.size(); i++ )
    {
        if (boost::iequals(auth_vector[i][1], auth))
        {
        	return auth_vector[i][0];
        }
    }
    return "NULL";
}

int DatabaseData::GetOaccessByAuth(std::string auth)
{
    for ( unsigned int i = 0 ; i < auth_vector.size(); i++ )
    {
        if (boost::iequals(auth_vector[i][1], auth))
        {
        	return convertString(auth_vector[i][2]);
        }
    }
    return -1;
}

int DatabaseData::GetGodByAuth(std::string auth)
{
    for ( unsigned int i = 0 ; i < auth_vector.size(); i++ )
    {
        if (boost::iequals(auth_vector[i][1], auth))
        {
        	return convertString(auth_vector[i][3]);
        }
    }
    return -1;
}

std::string DatabaseData::GetLanguageByAuth(std::string auth)
{
    for ( unsigned int i = 0 ; i < auth_vector.size(); i++ )
    {
        if (boost::iequals(auth_vector[i][1], auth))
        {
        	return auth_vector[i][4];
        }
    }
    return "NULL";
}

void DatabaseData::QueryRun()
{
    database *db;
    int state;
	unsigned int wait_time = 5;
	unsigned int counter = wait_time;
	while(mRun)
	{
		boost::mutex::scoped_lock lock(SqlMutex);
		while(sql_queue.empty() && mRun)
		{
			SqlAvailable.wait(lock);
		}
		if (counter >= wait_time)
		{
			db = new database();
			state = db->openConnection(mHostName.c_str(), mDatabaseName.c_str(), mUserName.c_str(), mPass.c_str());
		}
		if (state == 200)
		{
			while(!sql_queue.empty() && mRun)
			{
				std::string temp = sql_queue.front();
				sql_queue.pop();
				if (state == 200)
				{

					std::cout << temp << std::endl;
					db->updateQuery( temp.c_str() );
				}
				else
				{
					break;
				}
			}
		}
		if(sql_queue.empty() && mRun)
		{
			counter = 0;
			while (counter < wait_time && sql_queue.empty())
			{
				usleep(1000000);
				counter++;
			}
			if (counter >= wait_time)
			{
				db->disconnect();
				delete db;
			}
		}
	}
	db->disconnect();
	delete db;
}




std::vector< std::vector< std::string > > DatabaseData::RawSqlSelect(std::string data)
{
    std::cout << data << std::endl;
    database *db;
    std::vector< std::vector<std::string> > sql_result;
    db = new database();    // lol whut... connecting for each query? :'D
    int state = db->openConnection(mHostName.c_str(), mDatabaseName.c_str(), mUserName.c_str(), mPass.c_str());
    if (state == 200)
    {
        sql_result = db->sql_query( data.c_str() );
    }
    else
    {
        std::cout << mHostName << std::endl;
        std::cout << mDatabaseName << std::endl;
        std::cout << mUserName << std::endl;
        std::cout << mPass << std::endl;
        std::cout << "db fail " << state << std::endl;
    }
    db->disconnect();
    delete db;
    return sql_result;
}




std::string DatabaseData::convertInt(int number)
{
    std::stringstream ss;//create a stringstream
    ss << number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}

int DatabaseData::convertString(std::string data)
{
    int i;
    std::stringstream ss(data);//create a stringstream
    ss >> i;//add number to the stream
    return i;//return a string with the contents of the stream
}
