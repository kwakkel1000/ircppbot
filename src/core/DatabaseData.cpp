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
	channels_vector.clear();
	users_vector.clear();

    sql_string = "select auth.UserUuid, auth.auth, auth.oaccess, auth.god, auth.language from auth";
    auth_vector = RawSqlSelect(sql_string);

    sql_string = "select channels.ChannelUuid, channels.channel, channels.giveops, channels.givevoice from channels;";
    channels_vector = RawSqlSelect(sql_string);

    sql_string = "select users.ChannelUuid, users.UserUuid, users.access from users;";
    users_vector = RawSqlSelect(sql_string);
    std::cout << "DatabaseInit() DONE" << std::endl;
}

void DatabaseData::AddAuth(std::string mUserUuid, std::string mAuth)
{
	std::vector< std::string > tmp_auth;
	tmp_auth.clear();
	tmp_auth.push_back(mUserUuid);
	tmp_auth.push_back(mAuth);
	tmp_auth.push_back("-1");
	tmp_auth.push_back("-1");
	auth_vector.push_back(tmp_auth);
	std::string sqlstring = "INSERT into auth (UserUuid, auth) VALUES ( '" + mUserUuid + "', '" + mAuth + "' );";
	//std::cout << sqlstring << std::endl;
    //boost::mutex::scoped_lock lock(SqlMutex);
    sql_queue.push(sqlstring);
    SqlAvailable.notify_one();
}

void DatabaseData::AddChannel(std::string mChannelUuid, std::string mChannel)
{
	std::vector< std::string > tmp_channel;
	tmp_channel.clear();
	tmp_channel.push_back(mChannelUuid);
	tmp_channel.push_back(mChannel);
	tmp_channel.push_back("300");
	tmp_channel.push_back("300");
	channels_vector.push_back(tmp_channel);

	std::string sqlstring = "INSERT into channels ( ChannelUuid, channel ) VALUES ( '" + mChannelUuid + "', '" + mChannel + "' );";
	//std::cout << sqlstring << std::endl;
    //boost::mutex::scoped_lock lock(SqlMutex);
    sql_queue.push(sqlstring);
    SqlAvailable.notify_one();
}

void DatabaseData::DeleteChannel(std::string mChannelUuid)
{
    for ( unsigned int i = users_vector.size()-1; i >= 0; i-- )
    {
		if (users_vector[i].size() >= 1)
		{
			if (boost::iequals(users_vector[i][0], mChannelUuid))
			{
				users_vector.erase(users_vector.begin()+i);
			}
		}
    }
    for ( unsigned int i = channels_vector.size()-1; i >= 0; i-- )
    {
		if (channels_vector[i].size() >= 2)
		{
			if (boost::iequals(channels_vector[i][0], mChannelUuid))
			{
				channels_vector.erase(channels_vector.begin()+i);
			}
		}
    }

	std::string sqlstring;

	sqlstring = "DELETE from users where ChannelUuid = '" + mChannelUuid + "';";
	//std::cout << sqlstring << std::endl;
    //boost::mutex::scoped_lock lock(SqlMutex);
    sql_queue.push(sqlstring);
    SqlAvailable.notify_one();

	sqlstring = "DELETE from channels where ChannelUuid = '" + mChannelUuid + "';";
	//std::cout << sqlstring << std::endl;
    //boost::mutex::scoped_lock lock(SqlMutex);
    sql_queue.push(sqlstring);
    SqlAvailable.notify_one();
}

void DatabaseData::AddUserToChannel(std::string mChannelUuid, std::string mUserUuid, int mAccess)
{
	std::vector< std::string > tmp_user;
	tmp_user.clear();
	tmp_user.push_back(mChannelUuid);
	tmp_user.push_back(mUserUuid);
	tmp_user.push_back(convertInt(mAccess));
	users_vector.push_back(tmp_user);

	std::string sqlstring = "INSERT into users ( UserUuid, ChannelUuid, access) VALUES ( '" + mUserUuid + "', '" + mChannelUuid + "', '" + convertInt(mAccess) + "' );";
	//std::cout << sqlstring << std::endl;
    //boost::mutex::scoped_lock lock(SqlMutex);
    sql_queue.push(sqlstring);
    SqlAvailable.notify_one();
}

void DatabaseData::DeleteUserFromChannel(std::string mChannelUuid, std::string mUserUuid)
{
    for ( unsigned int i = 0 ; i < users_vector.size(); i++ )
    {
		if (users_vector[i].size() >= 2)
		{
			if (boost::iequals(users_vector[i][0], mChannelUuid) && boost::iequals(users_vector[i][1], mUserUuid))
			{
				users_vector.erase(users_vector.begin()+i);
			}
		}
    }

	std::string sqlstring = "DELETE from users where UserUuid = '" + mUserUuid + "' AND ChannelUuid = '" + mChannelUuid + "';";
	//std::cout << sqlstring << std::endl;
    //boost::mutex::scoped_lock lock(SqlMutex);
    sql_queue.push(sqlstring);
    SqlAvailable.notify_one();
}

std::string DatabaseData::GetUserUuidByAuth(std::string auth)
{
    for ( unsigned int i = 0 ; i < auth_vector.size(); i++ )
    {
		if (auth_vector[i].size() >= 2)
		{
			if (boost::iequals(auth_vector[i][1], auth))
			{
				return auth_vector[i][0];
			}
		}
    }
    return "NULL";
}

std::string DatabaseData::GetAuthByUserUuid(std::string UserUuid)
{
    for ( unsigned int i = 0 ; i < auth_vector.size(); i++ )
    {
		if (auth_vector[i].size() >= 2)
		{
			if (boost::iequals(auth_vector[i][0], UserUuid))
			{
				return auth_vector[i][1];
			}
		}
    }
    return "NULL";
}

int DatabaseData::GetOaccessByAuth(std::string auth)
{
    for ( unsigned int i = 0 ; i < auth_vector.size(); i++ )
    {
		if (auth_vector[i].size() >= 3)
		{
			if (boost::iequals(auth_vector[i][1], auth))
			{
				return convertString(auth_vector[i][2]);
			}
		}
    }
    return -1;
}

int DatabaseData::GetGodByAuth(std::string auth)
{
    for ( unsigned int i = 0 ; i < auth_vector.size(); i++ )
    {
		if (auth_vector[i].size() >= 4)
		{
			if (boost::iequals(auth_vector[i][1], auth))
			{
				return convertString(auth_vector[i][3]);
			}
		}
    }
    return -1;
}

std::string DatabaseData::GetLanguageByAuth(std::string auth)
{
    for ( unsigned int i = 0 ; i < auth_vector.size(); i++ )
    {
		if (auth_vector[i].size() >= 5)
		{
			if (boost::iequals(auth_vector[i][1], auth))
			{
				return auth_vector[i][4];
			}
		}
    }
    return "NULL";
}

std::string DatabaseData::GetChannelUuidByChannel(std::string channel)
{
    for ( unsigned int i = 0 ; i < channels_vector.size(); i++ )
    {
		if (channels_vector[i].size() >= 2)
		{
			if (boost::iequals(channels_vector[i][1], channel))
			{
				return channels_vector[i][0];
			}
		}
    }
    return "NULL";
}

std::string DatabaseData::GetChannelByChannelUuid(std::string ChannelUuid)
{
    for ( unsigned int i = 0 ; i < channels_vector.size(); i++ )
    {
		if (channels_vector[i].size() >= 2)
		{
			if (boost::iequals(channels_vector[i][0], ChannelUuid))
			{
				return channels_vector[i][1];
			}
		}
    }
    return "NULL";
}

int DatabaseData::GetGiveOpsByChannel(std::string channel)
{
    for ( unsigned int i = 0 ; i < channels_vector.size(); i++ )
    {
		if (channels_vector[i].size() >= 3)
		{
			if (boost::iequals(channels_vector[i][1], channel))
			{
				return convertString(channels_vector[i][2]);
			}
		}
    }
    return 501;
}

int DatabaseData::GetGiveVoiceByChannel(std::string channel)
{
    for ( unsigned int i = 0 ; i < channels_vector.size(); i++ )
    {
		if (channels_vector[i].size() >= 4)
		{
			if (boost::iequals(channels_vector[i][1], channel))
			{
				return convertString(channels_vector[i][3]);
			}
        }
    }
    return 501;
}

std::vector< std::vector< std::string > > DatabaseData::GetUserUuidAndAccessByChannelUuid(std::string ChannelUuid)
{
	std::vector< std::vector< std::string > > return_vector;
    for ( unsigned int i = 0 ; i < users_vector.size(); i++ )
    {
		if (users_vector[i].size() >= 3)
		{
			if (boost::iequals(users_vector[i][0], ChannelUuid))
			{
				std::vector< std::string > tmp;
				tmp.push_back(users_vector[i][1]);
				tmp.push_back(users_vector[i][2]);
				return_vector.push_back(tmp);
			}
        }
    }
    if (return_vector.size() == 0)
    {
		std::vector< std::string > tmp;
		tmp.push_back("NULL");
		tmp.push_back("NULL");
		return_vector.push_back(tmp);
    }
    return return_vector;
}

std::vector< std::string > DatabaseData::GetAuths()
{
	std::vector< std::string > return_vector;
    for ( unsigned int i = 0 ; i < auth_vector.size(); i++ )
    {
    	if (auth_vector[i].size() >= 2)
    	{
			return_vector.push_back(auth_vector[i][1]);
    	}
    }
    return return_vector;
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
