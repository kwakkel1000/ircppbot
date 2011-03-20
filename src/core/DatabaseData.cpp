#include "../include/core/DatabaseData.h"
#include "../include/core/Database.h"
#include "../include/core/Global.h"
#include "../include/interfaces/ConfigReaderInterface.h"
#include <boost/algorithm/string.hpp>

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
}

void DatabaseData::QueryRun()
{
	;
}
