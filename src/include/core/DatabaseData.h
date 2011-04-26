//
//
//  @ Project : ircppbot
//  @ File Name : DatabaseData.h
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


#ifndef SRC_INCLUDE_CORE_DATABASEDATA_H_
#define SRC_INCLUDE_CORE_DATABASEDATA_H_

#include <boost/bind.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>
#include <queue>
#include <map>

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

        void AddBinds(std::string mBindName);

        void AddAuth(std::string mUserUuid, std::string mAuth);

        void AddChannel(std::string mChannelUuid, std::string mChannel);
        void DeleteChannel(std::string mChannelUuid);

        void AddUserToChannel(std::string mChannelUuid, std::string mUserUuid, int mAccess);
        void DeleteUserFromChannel(std::string mChannelUuid, std::string mUserUuid);

        std::vector< std::string > GetBindVectorByBindName(std::string mBindName);
        std::string GetCommandByBindNameAndBind(std::string mBindName, std::string mBind);
        int GetAccessByBindNameAndBind(std::string mBindName, std::string mBind);

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

        // vars
        bool mRun;
        std::queue< std::string > sql_queue;

        // database_results
        std::vector< std::vector< std::string > > auth_vector;
        std::vector< std::vector< std::string > > channels_vector;
        std::vector< std::vector< std::string > > users_vector;
        std::map< std::string, std::vector< std::vector< std::string > > > binds_map;
        std::map< std::string, std::vector < std::string > > binds_vector;
        std::map< std::string, std::map < std::string, std::string > > binds_command_map;
        std::map< std::string, std::map < std::string, int > > binds_access_map;

        // config vars
        std::string mHostName;
        std::string mDatabaseName;
        std::string mUserName;
        std::string mPass;

        // thread vars
        boost::shared_ptr< boost::thread > mQueryThread;
        boost::condition SqlAvailable;
        boost::mutex SqlMutex;

        // functions
        void AddSqlQueue(std::string mSqlString);
        std::vector< std::vector< std::string > > RawSqlSelect(std::string data);
        std::string convertInt(int number);
        int convertString(std::string data);
        void QueryRun();
};

#endif  // SRC_INCLUDE_CORE_DATABASEDATA_H_
