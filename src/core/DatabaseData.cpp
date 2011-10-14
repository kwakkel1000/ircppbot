//
//
//  @ Project : ircppbot
//  @ File Name : DatabaseData.cpp
//  @ Date : 4/18/2011
//  @ Author : Gijs Kwakkel
//
//
// Copyright (c) 2011 Gijs Kwakkel
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//

#include "../include/core/DatabaseData.h"

#include <boost/algorithm/string.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../include/core/Database.h"
#include "../include/core/Global.h"
#include "../include/core/Output.h"
#include "../include/interfaces/ConfigReaderInterface.h"

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
    mRun = true;
}

/**
 * GetData
 * Create sql string from input vars and execute query
 * @param msWhere Which datasource to get the data from (database in this class)
 * @param msKey Which key to get the data from (colom in this class)
 * @param msCondition Which conditions has the data to fullfill to get in the return (Where in this class)
 * @return result of data in a vector
 *
 */
std::vector< std::string > DatabaseData::GetData(std::string msWhere, std::string msKey, std::string msCondition)
{
    std::string _sSqlString;
    _sSqlString = _sSqlString + "SELECT ";
    _sSqlString = _sSqlString + " `";
    _sSqlString = _sSqlString + msKey;
    _sSqlString = _sSqlString + "` FROM `";
    _sSqlString = _sSqlString + msWhere;
    _sSqlString = _sSqlString + "` WHERE ";
    _sSqlString = _sSqlString + msCondition;
    Output::Instance().addOutput(_sSqlString, 5);
    database *db;
    std::vector< std::vector< std::string > > _vSqlResult;
    std::vector< std::string > _vDataResult;
    db = new database();
    int state = db->openConnection(mHostName.c_str(), mDatabaseName.c_str(), mUserName.c_str(), mPass.c_str());
    if (state == 200)
    {
        _vSqlResult = db->sql_query(_sSqlString.c_str());
        for (unsigned int _uiSqlResultIndex = 0; _uiSqlResultIndex < _vSqlResult.size(); _uiSqlResultIndex++)
        {
            if (_vSqlResult[_uiSqlResultIndex].size() == 1)
            {
                _vDataResult.push_back(_vSqlResult[_uiSqlResultIndex][0]);
            }
        }
    }
    else
    {
        Output::Instance().addOutput("Database Connection fail", 4);
    }
    db->disconnect();
    delete db;
    return _vDataResult;
}

bool DatabaseData::InsertData(std::string msWhere, std::string msKey, std::string msValue)
{
    std::string _sSqlString;
    _sSqlString = _sSqlString + "INSERT into ";
    _sSqlString = _sSqlString + msWhere;
    _sSqlString = _sSqlString + " (`";
    _sSqlString = _sSqlString + msKey;
    _sSqlString = _sSqlString + "`) VALUES ('";
    _sSqlString = _sSqlString + msValue;
    _sSqlString = _sSqlString + "')";
    Output::Instance().addOutput(_sSqlString, 5);
    AddSqlQueue(_sSqlString);
    return true;
    return false;
}

bool DatabaseData::InsertData(std::string msWhere, std::vector< std::string > mvKeys, std::vector< std::string > mvValues)
{
    std::string _sSqlString;
    _sSqlString = _sSqlString + "INSERT into ";
    _sSqlString = _sSqlString + msWhere;
    _sSqlString = _sSqlString + " (`";
    for (unsigned int uiKeysIndex = 0; uiKeysIndex < mvKeys.size() -1; uiKeysIndex++)
    {
        _sSqlString = _sSqlString + mvKeys[uiKeysIndex];
        _sSqlString = _sSqlString + "`, `";
    }
    if (mvKeys.size() >= 1)
    {
        _sSqlString = _sSqlString + mvKeys[mvKeys.size() -1];
    }
    _sSqlString = _sSqlString + "`) VALUES (";
    for (unsigned int uiValuesIndex = 0; uiValuesIndex < mvValues.size() -1; uiValuesIndex++)
    {
        _sSqlString = _sSqlString + "'" + mvValues[uiValuesIndex];
        _sSqlString = _sSqlString + "', ";
    }
    if (mvValues.size() >= 1)
    {
        _sSqlString = _sSqlString + "'" + mvValues[mvValues.size() -1] + "'";
    }
    Output::Instance().addOutput(_sSqlString, 5);
    AddSqlQueue(_sSqlString);
    return true;
    return false;
}

bool DatabaseData::UpdateData(std::string msWhere, std::string msKey, std::string msValue, std::string msCondition)
{
    std::string _sSqlString;
    _sSqlString = _sSqlString + "UPDATE ";
    _sSqlString = _sSqlString + msWhere;
    _sSqlString = _sSqlString + " SET `";
    _sSqlString = _sSqlString + msKey;
    _sSqlString = _sSqlString + "`='";
    _sSqlString = _sSqlString + msValue;
    _sSqlString = _sSqlString + "' WHERE ";
    _sSqlString = _sSqlString + msCondition;
    Output::Instance().addOutput(_sSqlString, 5);
    AddSqlQueue(_sSqlString);
    return true;
    return false;
}

bool DatabaseData::UpdateData(std::string msWhere, std::vector< std::string > mvKeys, std::vector< std::string > mvValues, std::string msCondition)
{
    std::string _sSqlString;
    _sSqlString = _sSqlString + "UPDATE ";
    _sSqlString = _sSqlString + msWhere;
    _sSqlString = _sSqlString + " SET `";
    for (unsigned int uiKeysIndex = 0; uiKeysIndex < mvKeys.size() -1; uiKeysIndex++)
    {
        _sSqlString = _sSqlString + mvKeys[uiKeysIndex];
        _sSqlString = _sSqlString + "`='";
        _sSqlString = _sSqlString + mvValues[uiKeysIndex];
        _sSqlString = _sSqlString + "', `";
    }
    if (mvKeys.size() >= 1 && mvValues.size() >= 1)
    {
        _sSqlString = _sSqlString + mvKeys[mvKeys.size() -1];
        _sSqlString = _sSqlString + "`='";
        _sSqlString = _sSqlString + mvValues[mvKeys.size() -1];
        _sSqlString = _sSqlString + "'";
    }
    _sSqlString = _sSqlString + " WHERE ";
    _sSqlString = _sSqlString + msCondition;
    Output::Instance().addOutput(_sSqlString, 5);
    AddSqlQueue(_sSqlString);
    return true;
    return false;
}

// deprecated becomes LoadBinds()
// command will be base.command split on first .
void DatabaseData::AddBinds(std::string mBindName)
{
    std::vector< std::vector< std::string > > tmp_bind_vector;
    std::string sql_string = "select command, bind, access from " + mBindName + ";";
    tmp_bind_vector = RawSqlSelect(sql_string);
    binds_map[mBindName] = tmp_bind_vector;
    std::vector<  std::string > tmp_binds_vector;
    std::string tmp_bind_bind;
    std::string tmp_bind_command;
    boost::to_lower(mBindName);
    unsigned int i;
    for (i = 0 ; i < tmp_bind_vector.size() ; i++)
    {
        tmp_bind_bind = tmp_bind_vector[i][1];
        tmp_bind_command = tmp_bind_vector[i][0];
        boost::to_lower(tmp_bind_bind);
        boost::to_lower(tmp_bind_command);
        tmp_binds_vector.push_back(tmp_bind_bind);
        binds_command_map[mBindName][tmp_bind_bind] = tmp_bind_command;
        binds_access_map[mBindName][tmp_bind_bind] = convertString(tmp_bind_vector[i][2]);
    }
    binds_vector[mBindName] = tmp_binds_vector;
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
    AddSqlQueue(sqlstring);
}

void DatabaseData::AddChannel(std::string mChannelUuid, std::string mChannel)
{
    std::vector< std::string > tmp_channel;
    tmp_channel.clear();
    tmp_channel.push_back(mChannelUuid);
    tmp_channel.push_back(mChannel);
    tmp_channel.push_back("300");
    tmp_channel.push_back("100");
    channels_vector.push_back(tmp_channel);

    std::string sqlstring = "INSERT into channels ( ChannelUuid, channel ) VALUES ( '" + mChannelUuid + "', '" + mChannel + "' );";
    AddSqlQueue(sqlstring);
}

void DatabaseData::DeleteChannel(std::string mChannelUuid)
{
    for ( unsigned int i = users_vector.size(); i > 0; i-- )
    {
        if (users_vector[i-1].size() >= 1)
        {
            if (boost::iequals(users_vector[i-1][0], mChannelUuid))
            {
                users_vector.erase(users_vector.begin()+(i-1));
            }
        }
    }
    for ( unsigned int i = channels_vector.size(); i > 0; i-- )
    {
        if (channels_vector[i-1].size() >= 2)
        {
            if (boost::iequals(channels_vector[i-1][0], mChannelUuid))
            {
                channels_vector.erase(channels_vector.begin()+(i-1));
            }
        }
    }

    std::string sqlstring;

    sqlstring = "DELETE from users where ChannelUuid = '" + mChannelUuid + "';";
    AddSqlQueue(sqlstring);

    sqlstring = "DELETE from channels where ChannelUuid = '" + mChannelUuid + "';";
    AddSqlQueue(sqlstring);
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
    AddSqlQueue(sqlstring);
}

void DatabaseData::DeleteUserFromChannel(std::string mChannelUuid, std::string mUserUuid)
{
    for ( unsigned int i = users_vector.size() ; i > 0 ; i-- )
    {
        if (users_vector[i-1].size() >= 2)
        {
            if (boost::iequals(users_vector[i-1][0], mChannelUuid) && boost::iequals(users_vector[i-1][1], mUserUuid))
            {
                users_vector.erase(users_vector.begin()+(i-1));
            }
        }
    }

    std::string sqlstring = "DELETE from users where UserUuid = '" + mUserUuid + "' AND ChannelUuid = '" + mChannelUuid + "';";
    AddSqlQueue(sqlstring);
}

std::vector< std::string > DatabaseData::GetBindVectorByBindName(std::string mBindName)
{
    boost::to_lower(mBindName);
    return binds_vector[mBindName];
}

std::string DatabaseData::GetCommandByBindNameAndBind(std::string mBindName, std::string mBind)
{
    boost::to_lower(mBindName);
    boost::to_lower(mBind);
    return binds_command_map[mBindName][mBind];
}

int DatabaseData::GetAccessByBindNameAndBind(std::string mBindName, std::string mBind)
{
    boost::to_lower(mBindName);
    boost::to_lower(mBind);
    return binds_access_map[mBindName][mBind];
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

void DatabaseData::AddSqlQueue(std::string mSqlString)
{
    // std::cout << sqlstring << std::endl;
    // boost::mutex::scoped_lock lock(SqlMutex);
    sql_queue.push(mSqlString);
    SqlAvailable.notify_one();
}

void DatabaseData::QueryRun()
{
    database *db;
    db = new database();
    int state;
    unsigned int wait_time = 600;
    unsigned int counter = wait_time;
    while (!mRun)
    {
        usleep(100000);
    }
    std::cout << "QueryRun started" << std::endl;
    while (mRun)
    {
        boost::mutex::scoped_lock lock(SqlMutex);
        while (sql_queue.empty() && mRun)
        {
            SqlAvailable.wait(lock);
        }
        if (counter >= wait_time)
        {
            while (!db->connected())
            {
                std::cout << "open connection" << std::endl;
                state = db->openConnection(mHostName.c_str(), mDatabaseName.c_str(), mUserName.c_str(), mPass.c_str());
            }
        }
        if (state == 200 && db->connected())
        {
            while (!sql_queue.empty() && mRun)
            {
                std::string temp = sql_queue.front();
                sql_queue.pop();
                if (state == 200)
                {
                    std::cout << temp << std::endl;
                    db->updateQuery(temp.c_str());
                }
                else
                {
                    break;
                }
            }
            if (sql_queue.empty() && mRun)
            {
                counter = 0;
                while (counter < wait_time && sql_queue.empty())
                {
                    usleep(100000);
                    counter++;
                }
                if (counter >= wait_time)
                {
                    std::cout << "connection closed" << std::endl;
                    db->disconnect();
                    // delete db;
                }
            }
        }
    }
    if (db)
    {
        db->disconnect();
    }
    delete db;
}




std::vector< std::vector< std::string > > DatabaseData::RawSqlSelect(std::string data)
{
    std::cout << data << std::endl;
    database *db;
    std::vector< std::vector< std::string > > sql_result;
    db = new database();    // lol whut... connecting for each query? :'D
    int state = db->openConnection(mHostName.c_str(), mDatabaseName.c_str(), mUserName.c_str(), mPass.c_str());
    if (state == 200)
    {
        sql_result = db->sql_query(data.c_str());
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
    std::stringstream ss;
    ss << number;
    return ss.str();
}

int DatabaseData::convertString(std::string data)
{
    int i;
    std::stringstream ss(data);
    ss >> i;
    return i;
}
